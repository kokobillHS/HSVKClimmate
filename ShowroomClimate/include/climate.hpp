/*
 * climate.h
 *
 * This module handles state changes at all possible sources: 
 * modbus registers, radiators, thermostats
 *
 * Created: 19.05.2017 19:30:53
 *  Author: dmitry
 */ 


#ifndef CLIMATE_H_
#define CLIMATE_H_

enum  SourceType { SOURCE_RADIATOR, SOURCE_THERMOSTAT, SOURCE_MODBUS };

enum  ChangeFlags { 
    CF_CURRENT_MODE,
    CF_CURRENT_SP,
    CF_TIMER_ACTIVE,
    CF_EEPROM_MODE,

};
/*
class Radiator {
    public:
        Radiator();
        bool isValid();
        bool getInternalSensorAlarm();
        bool getExternalSensorAlarm();
        bool getOverheatingAlarm();
        bool getClockAlarm();
        bool getKeyboardLocked();
        bool getTimerActive();
        uint8_t getCurrentMode();
        uint8_t getCurrentSetpoint();
        uint8_t getSetpointForMode(uint8_t mode);
        uint8_t getRoomTemperature();
        void extractSchedule(const uint8_t *modes);
        void setTime(uint8_t dow, uint8_t h, uint8_t m, uint8_t s);
    private:
        uint8_t mode;
        uint8_t eepromMode;
        int8_t roomTemperature;
        int8_t thermostatRoomTemperature;
        int8_t thermostatSetpoint;
        int8_t setpoint[4];
        uint8_t scheduledModes[6];
        bool timerActive;
        bool keyboardLocked;
        bool internalSensorAlarm;
        bool externalSensorAlarm;
        bool overheatingAlarm;
        bool clockAlarm;
        bool valid;
        uint32_t lastResponse;
        uint32_t lastThermostatResponse;
        uint32_t changeFlags;
};
*/
#endif /* CLIMATE_H_ */