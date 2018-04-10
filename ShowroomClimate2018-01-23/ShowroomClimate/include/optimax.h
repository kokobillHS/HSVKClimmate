/*
 * optimax.h
 *
 * Created: 28.04.2017 11:20:42
 *  Author: dmitry
 */ 


#ifndef OPTIMAX_H_
#define OPTIMAX_H_

#include <avr/io.h>

#define ROOMS_COUNT 5

#define REQUEST_SIZE_RAD 10
#define RESPONSE_SIZE_RAD 10
#define REQUEST_SIZE_TS 7
#define RESPONSE_SIZE_TS 7
#define THERMOSTAT_ADDR_PREFIX 0x80
#define POLLING_PERIOD_MS 800UL
#define WATCHDOG_TIMEOUT 60000UL
#define MAX_RETRIES 1


// protocol markers
#define REQUEST_MARKER_RAD 0xDB
#define RESPONSE_MARKER_RAD 0xBD
#define REQUEST_MARKER_TS 0xCB
#define RESPONSE_MARKER_TS 0xBC

// status byte masks
#define MASK_MODE 0b00000011
#define MASK_TIMER_EN 0b00000100
#define MASK_LOCK 0b00001000
#define MASK_ERR_SENS_INTERN 0b00010000
#define MASK_ERR_SENS_ROOM 0b00100000
#define MASK_ERRTIME 0b01000000
#define MASK_OVERHEAT 0b10000000

enum DeviceType {
    RADIATOR,
    THERMOSTAT
};

enum ActionType {
    READ_T_SP_STATUS = 1,
    WRITE_SP_STATUS = 2,
    READ_SCHEDULE = 3,
    WRITE_SCHEDULE = 4,
    TIME_SYNC = 0xFF
};

enum RegisterOffset {
    REG_MODE,
    REG_CURRENT_T,
    REG_SP_0,
    REG_SP_1,
    REG_SP_2,
    REG_SP_3,
    REG_SCHEDULE_EN,
    REG_SCHEDULE_0_3,
    REG_SCHEDULE_4_7,
    REG_SCHEDULE_8_11,
    REG_SCHEDULE_12_15,
    REG_SCHEDULE_16_19,
    REG_SCHEDULE_20_23,
    REGS_COUNT
};

struct RoomData {
    uint8_t statusByte;
    uint8_t currentTemp;
    uint8_t setpoints[4];
    bool statusChanged;
    bool spChangedForTS;
	bool spChangedForRadiator;
    bool modeChanged;
    bool radiatorTimeout;
    bool thermostatTimeout;
};

void setCRC(uint8_t *dest, uint8_t size);

//void createRequest(uint8_t *dest, uint8_t addr, DeviceType devType, ActionType action, TemperatureData data, uint8_t status);
//void processResponse(uint8_t *dest);
//void processThermostat(uint8_t roomNo);
//void processRadiator(uint8_t roomNo);



#endif /* OPTIMAX_H_ */