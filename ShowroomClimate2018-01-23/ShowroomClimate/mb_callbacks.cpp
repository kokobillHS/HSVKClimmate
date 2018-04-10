/*
 * CPPFile1.cpp
 *
 * Created: 27.07.2016 13:43:00
 *  Author: dmitry
 */ 

#define F_OSC 8000000

#include <avr/io.h>
#include "MB/include/mb.h"
#include "MB/include/mbutils.h"
#include "include/mbregs.h"

eMBErrorCode eMBRegInputCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs) {
    eMBErrorCode    eStatus = MB_ENOREG;
    
    return eStatus;
}

eMBErrorCode eMBRegHoldingCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode) {
    eMBErrorCode    eStatus = MB_ENOERR;

    if (eMode == MB_REG_READ) {
        for (uint16_t i = 0; i < usNRegs; i++) {
            pucRegBuffer[i * 2] = readRegister(i + usAddress - 1) >> 8;
            pucRegBuffer[i * 2 + 1] = readRegister(i + usAddress - 1) & 0xFF;
        }
    } else if (eMode == MB_REG_WRITE) {
        for (uint16_t i = 0; i < usNRegs; i++) {
            uint16_t regValue = (pucRegBuffer[i * 2] << 8) | (pucRegBuffer[i * 2 + 1]);
            writeRegister(i + usAddress - 1, regValue);
        }
    }
    
    return eStatus;
}

eMBErrorCode eMBRegCoilsCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode) {
    eMBErrorCode    eStatus = MB_ENOREG;

    return eStatus;
}

eMBErrorCode eMBRegDiscreteCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNDiscrete) {
    eMBErrorCode    eStatus = MB_ENOREG;

    return eStatus; 
}