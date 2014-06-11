/* 
 * File:   main.h
 * Author: markr_000
 *
 * Created on 11 January 2013, 18:32
 */

#ifndef MAIN_H
#define	MAIN_H

#ifdef	__cplusplus
extern "C" {
#endif

#define CONFIGURATION_BIT_MASK 0x01

// Using Internal Clock of 8 MHz
#define OSC_FREQUENCY 8000000L
// Delay Function
#define	_delay_us(x) { unsigned char us; \
		       us = (x)/(12000000/OSC_FREQUENCY)|1; \
		       while(--us != 0) continue; }

void _delay_ms(unsigned int ms);

void init(void);
void setRGB( int R, int G, int B );

#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */

