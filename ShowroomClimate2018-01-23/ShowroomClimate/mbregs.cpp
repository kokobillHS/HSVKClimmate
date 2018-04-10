/*
 * mbmaster.cpp
 *
 * Created: 21.11.2016 16:52:38
 *  Author: dmitry
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "include/mbregs.h"
#include "include/optimax.h"

#define COMMSTAT_REGS_COUNT 20
#define COMMSTAT_REGS_START 200

enum CommDeviceType {
	DEV_RADIATOR,
	DEV_BATTERY,
	DEV_TYPE_COUNT
};

extern RoomData roomData[ROOMS_COUNT];

uint16_t serviceRegs[COMMSTAT_REGS_COUNT];

void writeRegister(uint16_t address, uint16_t value) {
    uint16_t roomWideAddr = address;
    uint8_t room = 0;

	if ((address >= COMMSTAT_REGS_START) && (address - COMMSTAT_REGS_START < COMMSTAT_REGS_COUNT)) {
		serviceRegs[address - COMMSTAT_REGS_START] = value;
		return;
	}

    while (roomWideAddr >= REGS_COUNT) {
        roomWideAddr -= REGS_COUNT;
        room++;
    }

    switch (roomWideAddr) {
        case REG_MODE:
            if ((roomData[room].statusByte & MASK_MODE) != (value & MASK_MODE)) {
                roomData[room].spChangedForTS = true;
                roomData[room].statusChanged = true;
            }
            
            roomData[room].statusByte &= ~MASK_MODE; // clear mode
            roomData[room].statusByte |= value & MASK_MODE; // set mode
            break;            
        case REG_SP_0:
            roomData[room].setpoints[0] = value & 0xFF;
            
            if ((roomData[room].statusByte & MASK_MODE) == 0) {
                roomData[room].spChangedForTS = true;
            }
            
			roomData[room].spChangedForRadiator = true;
            break;
        case REG_SP_1:
            roomData[room].setpoints[1] = value & 0xFF;
            
            if ((roomData[room].statusByte & MASK_MODE) == 1) {
                roomData[room].spChangedForTS = true;
            }            
            
			roomData[room].spChangedForRadiator = true;
            break;
        case REG_SP_2:
            roomData[room].setpoints[2] = value & 0xFF;
            
            if ((roomData[room].statusByte & MASK_MODE) == 2) {
                roomData[room].spChangedForTS = true;
            }
            
			roomData[room].spChangedForRadiator = true;
            break;
        case REG_SP_3:
            roomData[room].setpoints[3] = value & 0xFF;
            
            if ((roomData[room].statusByte & MASK_MODE) == 3) {
                roomData[room].spChangedForTS = true;
            }
            
			roomData[room].spChangedForRadiator = true;
            break;
    }
}

uint16_t readRegister(uint16_t address) {
    uint16_t roomWideAddr = address;
    uint8_t room = 0;
	
	if ((address >= COMMSTAT_REGS_START) && (address - COMMSTAT_REGS_START < COMMSTAT_REGS_COUNT)) {
		if (address - COMMSTAT_REGS_START >= 10) {
            uint8_t room = address - COMMSTAT_REGS_START - 10;
            return ((uint8_t)(roomData[room].radiatorTimeout)) & ((uint8_t)(roomData[room].thermostatTimeout) << 1);
        } else {
            return serviceRegs[address - COMMSTAT_REGS_START];
        }
	}
   
    while (roomWideAddr >= REGS_COUNT) {
        roomWideAddr -= REGS_COUNT;
        room++;
    }
    
    switch (roomWideAddr) {
        case REG_MODE:
            return roomData[room].statusByte;
        case REG_CURRENT_T:
            return roomData[room].currentTemp;
        case REG_SP_0:
            return roomData[room].setpoints[0];
        case REG_SP_1:
            return roomData[room].setpoints[1];
        case REG_SP_2:
            return roomData[room].setpoints[2];
        case REG_SP_3:
            return roomData[room].setpoints[3];
    }

    return 0xFF;
}
/*
void writeCommStatusReg(uint8_t room, CommDeviceType devType, bool status) {
	uint8_t offset = room * DEV_TYPE_COUNT + devType;
	serviceRegs[0] &= ~ (1 << offset); // clear the bit anyways
	serviceRegs[0] |= (((uint16_t)status) << offset); // and set again if needed
}

uint16_t readCommStatusReg() {
	return serviceRegs[0];
}
*/