/*
 * rs485.cpp
 *
 * Created: 22.09.2016 16:59:09
 *  Author: dmitry
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "include/time.h"

#define BUFFER_SIZE 32 // only powers of 2 allowed!
#define TX_BUFFER_SIZE0 BUFFER_SIZE
#define RX_BUFFER_SIZE0 BUFFER_SIZE

#define DATA_REGISTER_EMPTY (1<<UDRE0)
#define RX_COMPLETE (1<<RXC0)
#define FRAMING_ERROR (1<<FE0)
#define PARITY_ERROR (1<<UPE0)
#define DATA_OVERRUN (1<<DOR0)

// USART0 Receiver buffer

char rx_buffer0[RX_BUFFER_SIZE0];

unsigned char rx_wr_index0=0, rx_rd_index0=0;
unsigned char rx_counter0=0;

// This flag is set on USART0 Receiver buffer overflow
bool rx_buffer_overflow0;


// USART0 Receiver interrupt service routine
ISR(USART0_RX_vect) {
    char status,data;
    status = UCSR0A;
    data = UDR0;
    if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN)) == 0) {
        rx_buffer0[rx_wr_index0++] = data;
        
        if (rx_wr_index0 == RX_BUFFER_SIZE0) {
            rx_wr_index0=0;
        }

        if (++rx_counter0 == RX_BUFFER_SIZE0) {
            rx_counter0=0;
            rx_buffer_overflow0=1;
        }        
    }
}

// Get a character from the USART0 Receiver buffer

char getchar0(void) {
    char data;
    
    if (rx_counter0 == 0)
        return 0xFF;

    data = rx_buffer0[rx_rd_index0++];

    if (rx_rd_index0 == RX_BUFFER_SIZE0) {
        rx_rd_index0 = 0;
    }
    
    cli();
    --rx_counter0;
    sei();

    return data;
}

// USART0 Transmitter buffer

char tx_buffer0[TX_BUFFER_SIZE0];

unsigned char tx_wr_index0=0,tx_rd_index0=0;
unsigned char tx_counter0=0;


// USART0 Transmitter interrupt service routine
ISR(USART0_TX_vect) {
    if (tx_counter0)
    {
        --tx_counter0;
        UDR0 = tx_buffer0[tx_rd_index0++];
        
        if (tx_rd_index0 == TX_BUFFER_SIZE0) 
            tx_rd_index0 = 0;
    } else {
        PORTE &= ~(1 << 2); // Enable RX
    }
}

// Write a character to the USART0 Transmitter buffer

void putchar0(char arg) {
    if (tx_counter0 == TX_BUFFER_SIZE0) {
        return;
    }

    cli();

    if (tx_counter0 || ((UCSR0A & DATA_REGISTER_EMPTY) == 0)) {
        tx_buffer0[tx_wr_index0++] = arg;

        if (tx_wr_index0 == TX_BUFFER_SIZE0) {
            tx_wr_index0 = 0;
        }

        ++tx_counter0;
    } else {
        PORTE |= (1 << 2);
        UDR0 = arg;
    }

    sei();
}


// my

#define LED_7_ON  PORTL |=(1<<6)        // Info 1
#define LED_7_OFF PORTL &=~(1<<6)
#define LED_8_ON  PORTL |=(1<<7)        // Info 2
#define LED_8_OFF PORTL &=~(1<<7)
#define LED_8_TGL PORTL ^= (1<<7)

inline void RS485PutChar(uint8_t character) {
    putchar0(character);
}

inline int16_t RS485GetChar() {
    if (rx_counter0)
        return getchar0();
    else
        return -1;
}

uint8_t RS485GetString(uint8_t *dest, uint8_t capacity) {
    uint8_t charCount = rx_counter0;
    charCount = (charCount > capacity) ? capacity : charCount;

    for (uint8_t i = 0; i < charCount; i++) {
        dest[i] = getchar0();
    }

    return charCount;
}

void RS485PutString(uint8_t *src, uint8_t count) {
    PORTE |= (1 << 2);

    for (uint8_t i = 0; i < count; i++) {
        RS485PutChar(src[i]);
    }
}

uint8_t RS485ReceivedCharsCount() {
    return rx_counter0;
}

void clearRxBuffer() {
    while (RS485GetChar() >= 0) /* NOP */;
}

void processTxEnPin() {
    if (tx_counter0) {
        PORTE |= (1 << 2);
    }
}