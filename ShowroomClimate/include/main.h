/*
 * main.h
 *
 * Created: 22.09.2016 16:16:11
 *  Author: dmitry
 */ 


#ifndef MAIN_H_
#define MAIN_H_

/**
 * 0 - write to thermostat (if necessary)
 * 1 - response
 * 2 - poll thermostat
 * 3 - response
 * 4 - poll radiator
 * 5 - response
 * 6 - write to radiator (if necessary)
 * 7 - response
 * 8 - time sync (ToDo)
 */
#define CYCLES_PER_ROOM 8

#define LED_1_ON  PORTL |= (1<<0)        // CPU run
#define LED_1_OFF PORTL &= ~(1<<0)
#define LED_1_TOGGLE PORTL ^= (1<<0)
#define LED_2_ON  PORTL |= (1<<1)        // CPU run
#define LED_2_OFF PORTL &= ~(1<<1)

#define IS_EVEN_OR_ZERO(x) (((x) & 1) == 0)
#define IS_ODD(x) (((x) & 1) != 0)

#define IS_REQUEST_CYCLE(x) IS_EVEN_OR_ZERO(x)
#define IS_RESPONSE_CYCLE(x) IS_ODD(x) 
#define IS_THERMOSTAT_CYCLE(x) ((x) < 4)
#define IS_RADIATOR_CYCLE(x) ((x) > 3)


void initPorts();


#endif /* MAIN_H_ */