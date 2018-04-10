/*
 * rs485.h
 *
 * Created: 22.09.2016 17:18:22
 *  Author: dmitry
 */ 


#ifndef RS485_H_
#define RS485_H_

uint8_t RS485ReceivedCharsCount();

void RS485PutChar(uint8_t character);
void RS485PutString(uint8_t *src, uint8_t count);

/** 
 * Reads a single character from Rx buffer if any is present.
 * Returns the read character or -1 if buffer is empty.
 */
int16_t RS485GetChar();

/** 
 * Reads a string from Rx buffer into *dest buffer with defined capacity.
 *
 * Returns number of characters that were actually written into *dest.
 *
 */
uint8_t RS485GetString(uint8_t *dest, uint8_t capacity);

/** 
 *   Clears RX buffer
 */
void clearRxBuffer();

void processTxEnPin();

#endif /* RS485_H_ */