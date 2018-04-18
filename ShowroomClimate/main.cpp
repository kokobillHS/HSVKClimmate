/*
 * ApartmentClimate.cpp
 *
 * Created: 22.09.2016 16:07:21
 * Author : dmitry
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "MB/include/mb.h"
#include "MB/include/mbport.h"
#include "include/main.h"
#include "include/time.h"
#include "include/rs485.h"
#include "include/mbregs.h"
#include "include/optimax.h"

RoomData roomData[ROOMS_COUNT];

bool responseCrcIsValid(uint8_t *response, uint8_t responseSize) {
    uint8_t crc = response[responseSize - 1];
    setCRC(response, responseSize);

    return (crc == response[responseSize - 1]);
}

bool validateResponse(uint8_t *response, uint8_t responseSize, uint8_t expectedResponseSize, uint8_t room, uint8_t &cycle, uint8_t &retries) {
    if ((responseSize != expectedResponseSize) || !responseCrcIsValid(response, responseSize)) {
        if (retries < MAX_RETRIES) {
            cycle--; // re-send
            retries++;
        } else {
            if (IS_THERMOSTAT_CYCLE(cycle)) {
                roomData[room].thermostatTimeout = true;
            } else {
                roomData[room].radiatorTimeout = true;
            }
            
            cycle++;
            retries = 0;
        }
        return false;
    } else {
        retries = 0;
    }

    return true;
}

void sendRequest(uint8_t room, uint8_t &cycle, uint8_t &expectedResponseSize, bool initial = false) {
    uint8_t request[REQUEST_SIZE_RAD] = { 0 };
    uint8_t requestSize = 0;
    uint8_t currentSP = roomData[room].setpoints[roomData[room].statusByte & MASK_MODE];

    memset(request + 1, 0, REQUEST_SIZE_RAD - 1); // clear request
    request[1] = room + 1;

    switch (cycle) {
        case 0: // write new SP (if any) to thermostat
            if (!roomData[room].spChangedForTS) {
                cycle = 2; // go to poll thermostat
                return;
            }
            requestSize = REQUEST_SIZE_TS;
            expectedResponseSize = RESPONSE_SIZE_TS;
            request[0] = REQUEST_MARKER_TS;
            request[1] |= THERMOSTAT_ADDR_PREFIX;
            request[2] = WRITE_SP_STATUS;
            request[5] = currentSP;
            break;

        case 2: // poll thermostat
            if (roomData[room].spChangedForTS) { // omit reading from TS if new SP from app is pending
                cycle = 4;
                return;
            }
        
            requestSize = REQUEST_SIZE_TS;
            expectedResponseSize = RESPONSE_SIZE_TS;
            request[0] = REQUEST_MARKER_TS;
            request[1] |= THERMOSTAT_ADDR_PREFIX;
            request[2] = READ_T_SP_STATUS;
            break;

        case 4: // poll radiator
            requestSize = REQUEST_SIZE_RAD;
            expectedResponseSize = RESPONSE_SIZE_RAD;
            request[0] = REQUEST_MARKER_RAD;
            request[2] = READ_T_SP_STATUS;
            request[4] = (initial ? 0xFF : roomData[room].currentTemp);
            break;

        case 6: // write to radiator
            if (!roomData[room].spChangedForRadiator && !roomData[room].statusChanged) {
                cycle = 8; // skip response processing as well
                return;
            }
        
            requestSize = REQUEST_SIZE_RAD;
            expectedResponseSize = RESPONSE_SIZE_RAD;
            request[0] = REQUEST_MARKER_RAD;
            request[2] = WRITE_SP_STATUS;
            request[3] = roomData[room].statusByte & MASK_MODE;
            request[5] = roomData[room].setpoints[0];
            request[6] = roomData[room].setpoints[1];
            request[7] = roomData[room].setpoints[2];
            request[8] = roomData[room].setpoints[3];
            break;
    }         
             
    setCRC(request, requestSize);
    RS485PutString(request, requestSize);
}


void processResponse(uint8_t room, uint8_t &cycle, uint8_t expectedResponseSize, bool initial = false) {    
    uint8_t response[RESPONSE_SIZE_RAD] = { 0 };
    uint8_t responseSize = RS485GetString(response, RESPONSE_SIZE_RAD);
    uint8_t currentSP = roomData[room].setpoints[roomData[room].statusByte & MASK_MODE];
    static uint8_t retries = 0;
    
    clearRxBuffer();
    writeRegister(209, retries);

    if (!validateResponse(response, responseSize, expectedResponseSize, room, cycle, retries)) {
        return;
    }

    if (IS_THERMOSTAT_CYCLE(cycle)) {
        roomData[room].thermostatTimeout = false;
    } else {
        roomData[room].radiatorTimeout = false;
    }

    switch (cycle) {
        case 1: // Thermostat SP change
            if (response[5] == currentSP) {
                roomData[room].spChangedForTS = false;
                cycle = 4; // skip reading SP from thermostat
                return;
            }
            break;

        case 3: // Poll thermostat
            if ((response[5] != currentSP) && (!roomData[room].spChangedForRadiator)) {
                roomData[room].spChangedForRadiator = true;
                roomData[room].setpoints[roomData[room].statusByte & MASK_MODE] = response[5];
            }
            roomData[room].currentTemp = response[4];
            break;
        
        case 5: // Poll radiator 
            if (initial) {
                roomData[room].statusByte = response[3];
                roomData[room].currentTemp = response[4];
                roomData[room].setpoints[0] = response[5];
                roomData[room].setpoints[1] = response[6];
                roomData[room].setpoints[2] = response[7];
                roomData[room].setpoints[3] = response[8];
            }
            break;
        
        case 7: // Write to radiator
            if (response[5 + (response[3] & MASK_MODE)] == currentSP) {
                roomData[room].spChangedForRadiator = false;
                roomData[room].statusChanged = false;
            }
            break;
    }
}

void processMainTask() {
    static uint8_t expectedResponseSize = 0;
    static uint8_t currentRoom = 0;
    static uint8_t currentCycle = 0;
    static uint32_t watchdogUpdateTime = 0;
    
    if (currentCycle >= CYCLES_PER_ROOM) {
        currentRoom++;
        currentCycle = 0;
    }

    if ((currentRoom >= ROOMS_COUNT) || (millis() - watchdogUpdateTime > WATCHDOG_TIMEOUT)) {
        currentRoom = 0;
        currentCycle = 0;
        watchdogUpdateTime = millis();
    }
    
    writeRegister(200, currentCycle);

    if (IS_REQUEST_CYCLE(currentCycle)) {
        uint8_t oldCycle = currentCycle;
        sendRequest(currentRoom, currentCycle, expectedResponseSize);
        
        if (oldCycle != currentCycle) {
            return;
        }
    } else {
        uint8_t oldCycle = currentCycle;
        processResponse(currentRoom, currentCycle, expectedResponseSize);

        if (oldCycle != currentCycle) {
            return;
        }
    }

    currentCycle++;
}

void readInitialValues() {
    uint32_t start = 0;
    
    for (uint8_t i = 0; i < ROOMS_COUNT; i++) {
        uint8_t cycle = 4;
        uint8_t expectedResponseSize = 0;
        uint8_t retries = 0;
        bool roomProcessed = false;
        
        while (!roomProcessed) {
            sendRequest(i, cycle, expectedResponseSize, true);
            start = millis();
        
            while (millis() - start < POLLING_PERIOD_MS) ;
        
            uint8_t oldCycle = cycle;
            cycle = 5;
            processResponse(i, cycle, expectedResponseSize, true);
            
            if ((oldCycle == cycle) || (retries >= MAX_RETRIES)) {
                roomProcessed = true;
            } else {
                retries++;
            }
        }   
    }  
}

int main() {
    uint32_t start = 0;
    
    initPorts();
    initTimer();

    eMBInit(MB_RTU, 0x02, 0, 9600, MB_PAR_NONE);
    eMBEnable();

    LED_1_ON;    
    
    readInitialValues();
    start = millis();
    
    while (1) {
        if (millis() - start > POLLING_PERIOD_MS) {
            start = millis();
            processMainTask();
            LED_1_TOGGLE;
        }

        processTxEnPin();
        eMBPoll();
    }
}
