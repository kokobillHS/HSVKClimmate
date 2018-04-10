/*
 * mbmaster.h
 *
 * Created: 21.11.2016 16:49:45
 *  Author: dmitry
 */ 


#ifndef MBMASTER_H_
#define MBMASTER_H_

void writeRegister(uint16_t address, uint16_t value);
uint16_t readRegister(uint16_t address);
//void writeCommStatusReg(uint8_t room, CommDeviceType devType, bool status);
uint16_t readCommStatusReg();

#endif /* MBMASTER_H_ */