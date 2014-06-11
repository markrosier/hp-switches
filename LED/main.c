/* 
 * File:   main.c
 * Author: markr_000
 *
 * Created on 24 December 2012, 17:58
 */

#include <stdio.h>
#include <stdlib.h>
#include "hardware.h"
#include "main.h"
#include "interrupt.h"

#pragma config (FOSC=INTOSC)            // Internal Oscillator
#pragma config (WDTE=OFF)	  	// Watchdog Timer Disabled
#pragma config (PWRTE=OFF)		// Power up timer disabled
#pragma config (MCLRE=OFF)		// MCLR pin is digital input
#pragma config (CP=OFF)			// No Code Protection
#pragma config (CPD=OFF)		// No data Memory Code Protection
#pragma config (BOREN=OFF)		// Brown out reset disabled
#pragma config (CLKOUTEN=OFF)            // CLK OUT function disabled
#pragma config (IESO=OFF)
#pragma config (FCMEN=OFF)
#pragma config (WRT=OFF)
#pragma config (PLLEN= ON)
#pragma config (STVREN=OFF)				
#pragma config (BORV=1)
#pragma config (LVP=OFF )

extern bool RGBReadytoSet;
extern unsigned char red, green, blue;
extern unsigned long noOfSlaveBits;

void _delay_ms(unsigned int ms)
{
  unsigned char i;
  if (ms == 0) return;
  do {
    i = 4;
    do {
      _delay_us(164);
    } while(--i);
  } while(--ms);
}

void initTimer2( void )
{
  OSCCON=0x70;         // Select 8 MHz internal clock
  T2CON=0b00000111;    // Postscale: 1:1, Timer2=On, Prescale = 1:64
  PR2 = 0xFF;
  TMR2=0;              // Start with zero Counter
//  PIE1bits.TMR2IE = 1;
}

void initTimer4( void )
{
  OSCCON=0x70;         // Select 8 MHz internal clock
  T4CON=0b00000111;    // Postscale: 1:1, Timer2=On, Prescale = 1:64
  PR4=0xFF;            
  TMR4=0;              // Start with zero Counter
//  PIE3bits.TMR4IE = 1;
}


void initBluePWM( void )
{
  BLUE_PWM_PIN_DIR = DISABLE_OUTPUT;
  CCP1CON=0b00001100;
  CCPR1L=0;            // Start with zero Duty Cycle
  PSTR1CON=0b00000001;
  BLUE_PWM_PIN_DIR = ENABLE_OUTPUT;
}

void setBluePWM( int dutyCycle )
{
	CCPR1L= dutyCycle & 0xFF;
}

void initGreenPWM( void )
{
  GREEN_PWM_PIN_DIR = DISABLE_OUTPUT;
  CCP3CON=0b00001100;
  CCPR2L=0;            // Start with zero Duty Cycle
  PSTR2CON=0b00000001;
  GREEN_PWM_PIN_DIR = ENABLE_OUTPUT;

}

void setGreenPWM( int dutyCycle )
{
	CCPR3L= dutyCycle & 0xFF;
}

void initRedPWM( void )
{
  RED_PWM_PIN_DIR = DISABLE_OUTPUT;
  CCP2CON=0b00001100;
  CCPR2L=0;            // Start with zero Duty Cycle
  PSTR2CON=0b00000001;
  RED_PWM_PIN_DIR = ENABLE_OUTPUT;
}


void setRedPWM( int dutyCycle )
{
	CCPR2L= dutyCycle & 0xFF;
}


void setRGB( int R, int G, int B )
{
  if ( (B & CONFIGURATION_BIT_MASK) == CONFIGURATION_BIT_MASK)
  {
    noOfSlaveBits = (R+1) * 24;
  }
  else
  {
    setRedPWM(R);
    setGreenPWM(G);
    setBluePWM(B);
  }
}

void initRGB( void )
{
	initTimer2();
//	initTimer4();
	initRedPWM();
	initGreenPWM();
	initBluePWM();
	setRGB(0,0,0);
}

void main(void)
{
  init();
  initRGB();
  initInterrupt();

  while (true)
 {
    if (RGBReadytoSet)
    {
      setRGB(red, green, blue);
      RGBReadytoSet = false;
    }
 }
}

void init( void )
{
  LED_DIR = OUTPUT_PIN;
  LED = LED_OFF;


	// LED Driver Pins
	RED_PWM_PIN_TYPE = DIGITAL_PIN;
	RED_PWM_PIN_DIR = OUTPUT_PIN;
	RED_PWM_LATCH = PWM_LED_OFF;
	GREEN_PWM_PIN_TYPE = DIGITAL_PIN;
  GREEN_PWM_LATCH = PWM_LED_OFF;
	GREEN_PWM_LATCH = PWM_LED_OFF;
	BLUE_PWM_PIN_DIR = OUTPUT_PIN;
	BLUE_PWM_LATCH = PWM_LED_OFF;

  RC4_NOT_USED_PIN_DIR = DISABLE_OUTPUT;

	// Data Pins
	DATA_IN_PIN_TYPE = DIGITAL_PIN;
	DATA_IN_PIN_DIR = INPUT_PIN;
  
	DATA_OUT_PIN_TYPE = DIGITAL_PIN;
	DATA_OUT_PIN_DIR = OUTPUT_PIN;
  DATA_OUT_LATCH = 0;

 	DC_DATA_OUT_PIN_TYPE = DIGITAL_PIN;
	DC_DATA_OUT_PIN_DIR = OUTPUT_PIN;
  DC_DATA_OUT_LATCH = 0;

	SPARE1_PIN_TYPE = DIGITAL_PIN;
	SPARE1_PIN_DIR = INPUT_PIN;
  SPARE1_LATCH = 0;


  // for test remove
// 	SPARE1_PIN_DIR = OUTPUT_PIN;

}




