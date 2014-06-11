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

#pragma config (FOSC=INTOSC)  	// Internal Oscillator
#pragma config (WDTE=OFF)	  	// Watchdog Timer Disabled
#pragma config (PWRTE=OFF)		// Power up timer disabled
#pragma config (MCLRE=OFF)		// MCLR pin is digital input
#pragma config (CP=OFF)			// No Code Protection
#pragma config (CPD=OFF)		// No data Memory Code Protection
#pragma config (BOREN=OFF)		// Brown out reset disabled
#pragma config (CLKOUTEN=OFF)	// CLK OUT function disabled 
#pragma config (IESO=OFF)
#pragma config (FCMEN=OFF)
#pragma config (WRT=OFF)
#pragma config (PLLEN= ON)
#pragma config (STVREN=OFF)				
#pragma config (BORV=1)
#pragma config (LVP=OFF )

extern bool RGBReadytoSet;
extern unsigned char red, green, blue;

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
  T2CON=0b00000111;    // Postscale: 1:1, Timer2=On, Prescale = 1:4
  PR2 = 0xFF;
  TMR2=0;              // Start with zero Counter
//  PIE1bits.TMR2IE = 1;
}

void initTimer4( void )
{
  OSCCON=0x70;         // Select 8 MHz internal clock
  T4CON=0b00000111;    // Postscale: 1:1, Timer2=On, Prescale = 1:4
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
  CCP4CON=0b00001100;
  CCPR4L=0;            // Start with zero Duty Cycle
//  PSTR4CON=0b00000001;
  GREEN_PWM_PIN_DIR = ENABLE_OUTPUT;

}

void setGreenPWM( int dutyCycle )
{
	CCPR4L = dutyCycle & 0xFF;
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
    setRedPWM(R);
    setGreenPWM(G);
    setBluePWM(B);
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

int getValue ( void )
{
	int val = 0;
	
	FVRCONbits.ADFVR = 0b10;  	// 2.048V Reference
	FVRCONbits.FVREN = 1;		// Enable fixed voltage reference
	ADCON1bits.ADPREF = 0b11;	// Select reference to be FVR
  ADCON1bits.ADCS = 0b001;  // set conversion to be 1uS
	BALL_DETECT_PIN_TYPE = ANALOGUE_PIN;
	BALL_DETECT_DIR = INPUT_PIN;
	ADCON0bits.CHS = BALL_DETECT_CHANNEL;	// set analogue channel
	ADCON0bits.ADON = 1;		// set ADC On
	ADCON0bits.GO_nDONE = 1;	// start the conversion
	while (ADCON0bits.GO_nDONE)
	{
		val ++;  // just do something so compiler doesnt optimise the loop out
	}
	val = ADRES >> 6;  //Move 10 bits to the right

	return val;
}

int calibrate( void )
{
	// read the value 10 times, if the value is within 10mV use it
	// otherwise discard it and take the average of the ten readings
	// as the quescent state
	int i;
	long cumulativeVal = 0;
	long averageVal;
	int inputVal;
	int lastVal = 0;

	// dont start until you get 2 values that are pretty similar
	// ie within a specified level
	// and within specified upper and lower limits of the expected value

	while (true)
	{
		inputVal = getValue();
		if ( (inputVal < HIGH_STEADY_STATE_LEVEL) 
				&& (inputVal > LOW_STEADY_STATE_LEVEL) )
		{
			lastVal = inputVal;
			inputVal = getValue();
			if ( (inputVal < HIGH_STEADY_STATE_LEVEL) 
					&& (inputVal > LOW_STEADY_STATE_LEVEL) )
			{
				if ( abs( inputVal - lastVal ) < STEADY_STATE_LEVEL_VARIATION )
				{
					cumulativeVal = inputVal;
					averageVal = inputVal;
					break;
				}
			}
		}
    _delay_ms(20);
	}
			
	i = 1;
	while (i < 10)
	{
		inputVal = getValue();
		if ( abs( inputVal - averageVal ) < STEADY_STATE_LEVEL_VARIATION )
		{
			cumulativeVal += inputVal;
			averageVal = cumulativeVal / (i + 1);
			i ++;
		}
    _delay_ms(20);
	}
		
	// signal that calibration has been successful
	for (i = 0; i < 3; i ++)
	{
		LED = LED_ON;
		_delay_ms(200);
		LED= LED_OFF;
		_delay_ms(200);
	}


	return averageVal;
}


void main(void)
{

  int steadyStateValue, lowerBallSensorThresholdValue, upperBallSensorThresholdValue;
  enum {offState, onState} state = offState;
  int value;

	init();
  _delay_ms(10);
  steadyStateValue = calibrate();
  int changeValue = ((long)steadyStateValue * BALL_DETECT_PERCENT_CHANGE)/100;
	lowerBallSensorThresholdValue = steadyStateValue - changeValue;
  int hysterysis = ((long)steadyStateValue * HYSTERYSIS_PERCENT)/100;
  upperBallSensorThresholdValue = lowerBallSensorThresholdValue + hysterysis;
  initInterrupt();
  initRGB();

  while (true)
  {
    if (RGBReadytoSet)
    {
      setRGB(red, green, blue);
      RGBReadytoSet = false;
    }
    value = getValue();
		switch (state)
		{
			case offState:
				if ( value < lowerBallSensorThresholdValue )
				{
				LED = LED_ON;
 				SENSOR_SWITCH_OUT_PIN = 0;
				state = onState;
				}
			break;
			case onState:
				if ( value > upperBallSensorThresholdValue)
				{
				LED = LED_OFF;
				SENSOR_SWITCH_OUT_PIN = 1;
				state = offState;
				}
			break;
		}
  }
}

void init( void )
{
  OSCCON=0x70;         // Select 8 MHz internal clock

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

  // ball sensor output pin
	SENSOR_SWITCH_OUT_PIN_TYPE = DIGITAL_PIN;
	SENSOR_SWITCH_OUT_PIN_DIR = OUTPUT_PIN;
	SENSOR_SWITCH_OUT_PIN = 1;

}




