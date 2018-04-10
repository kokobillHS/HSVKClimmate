/*
 * optimax.cpp
 *
 * Created: 28.04.2017 11:21:52
 *  Author: dmitry
 */ 
 
#include "include/optimax.h"
#include "include/time.h"

#define TELEGRAM_SIZE 10
#define THERMOSTAT_ADDR_PREFIX 0x80

const uint8_t REQUEST_GET_STATE = 1;
const uint8_t REQUEST_SET_STATE = 2;
const uint8_t REQUEST_GET_SCHEDULE = 3;
const uint8_t REQUEST_SET_SCHEDULE = 4;
const uint8_t REQUEST_SET_TIME = 0xFF;
/*
static bool scheduleUpdateScheduled = false;
static bool stateUpdateScheduled = false;

static uint8_t temperatures[NUM_ROOMS] = { 0 };
static uint8_t modes[NUM_ROOMS] = { 0 };


void createRequest(uint8_t *dest, uint8_t addr, DeviceType devType, ActionType action, TemperatureData data, uint8_t status) {
    dest[0] = 0xDB; // marker
    dest[1] = addr;
    dest[2] = action;
    
    memset(dest + 3, 0, REQUEST_SIZE - 3); // fill the rest with zeros

    if (action == READ_T_SP_STATUS) {
        dest[4] = data.currentTemp;
    }

    if (action == WRITE_SP_STATUS) {
        dest[3] = status;
        dest[5] = data.SP0;
        dest[6] = data.SP1;
        dest[7] = data.SP2;
        dest[8] = data.SP3;
    }

    //TODO: schedule & time sync

    for (uint8_t i = 0; i < CHECKSUM_POS; i++) {
        dest[CHECKSUM_POS] ^= dest[i];
    }
}

void processResponse(uint8_t *dest) {
    if (dest[0] != 0xBD) {
        return; // Marker error
    }

    DeviceType devType = RADIATOR;

    if (dest[1] & 0x80) {
        devType = THERMOSTAT;
    }

    uint8_t addr = dest[1] & 0x0F;
}*/

void setCRC(uint8_t *dest, uint8_t size) {
    uint8_t checksumIndex = size - 1;
    dest[checksumIndex] = 0;

    for (uint8_t i = 0; i < size - 1; i++) {
        dest[checksumIndex] ^= dest[i];
    }
}