/* 
 * File:   main.c
 * Author: markr_000
 *
 * Created on 24 December 2012, 17:58
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "hardware.h"
#include "main.h"
#include "interrupt.h"
#include "pic12f1822.h"

#pragma config (FOSC=INTOSC)  	// Internal Oscillator
#pragma config (WDTE=ON)	  	// Watchdog Timer Enabled
#pragma config (PWRTE=ON)		// Power up timer enabled
#pragma config (MCLRE=OFF)		// MCLR pin is digital input
#pragma config (CP=ON)			// Code Protection
#pragma config (CPD=ON)		// Data Memory Code Protection
#pragma config (BOREN=ON)		// Brown out reset enabled
#pragma config (CLKOUTEN=OFF)	// CLK OUT function disabled 
#pragma config (IESO=OFF)
#pragma config (FCMEN=OFF)
#pragma config (WRT=OFF)
#pragma config (PLLEN= ON)
#pragma config (STVREN=OFF)				
#pragma config (BORV=0)			//Vbor=2.7V
#pragma config (LVP=OFF )

long cumulativeVal = 0;

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

void init( void )
{
  WDTCONbits.WDTPS = WDT_PERIOD;
  OSCCON=0x70;         // Select 8 MHz internal clock
  LED_DIR = OUTPUT_PIN;
  LED = LED_OFF;
	SENSOR_SWITCH_OUT_PIN_TYPE = DIGITAL_PIN;
	SENSOR_SWITCH_OUT_PIN_DIR = OUTPUT_PIN;
	SENSOR_SWITCH_OUT_PIN = 1;

  	// Data Pins
	DATA_IN_PIN_TYPE = DIGITAL_PIN;
	DATA_IN_PIN_DIR = INPUT_PIN;

	DATA_OUT_PIN_TYPE = DIGITAL_PIN;
	DATA_OUT_PIN_DIR = OUTPUT_PIN;
  DATA_OUT_LATCH = 0;
}


int getValue ( void )
{
	//int val = 0;
	
	FVRCONbits.ADFVR = 0b10;  	// 2.048V Reference
	FVRCONbits.FVREN = 1;		// Enable fixed voltage reference
	ADCON1bits.ADPREF = 0b11;	// Select reference to be FVR
  ADCON1bits.ADCS = 0b001;  // set conversion to be 1uS
	BALL_DETECT_PIN_TYPE = ANALOGUE_PIN;
	BALL_DETECT_DIR = INPUT_PIN;
	ADCON0bits.CHS = BALL_DETECT_CHANNEL;	// set analogue channel
	ADCON0bits.ADON = 1;		// set ADC On
	ADCON0bits.GO_nDONE = 1;	// start the conversion
	while (ADCON0bits.GO_nDONE);
	/*{
		val ++;  // just do something so compiler doesnt optimise the loop out
	}*/
	return ADRES >> 6;  //Move 10 bits to the right

	//return val;
}

int calibrate( void )
{
	// read the value 10 times, if the value is within 10mV use it
	// otherwise discard it and take the average of the ten readings
	// as the quescent state
	char i;
	long averageVal;
	int inputVal;
	int lastVal = 0;

	// dont start until you get 2 values that are pretty similar
	// ie within a specified level
	// and within specified upper and lower limits of the expected value
	while (true)
	{
    CLRWDT();
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

  // No CLRWDT in this loop, to prevent a lockout when the switch cannot perform calibration
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
  // also signal on the switch line for the IOBoard
  // 2 pulses, 200ms each
	for (i = 0; i < 2; i ++)
	{
    CLRWDT();
		LED = LED_ON;
    SENSOR_SWITCH_OUT_PIN = 0;
		_delay_ms(200);

    CLRWDT();
		LED= LED_OFF;
    SENSOR_SWITCH_OUT_PIN = 1;
		_delay_ms(200);
	}

	return averageVal;
}

void main(void)
{
  int steadyStateValue, lowerBallSensorThresholdValue, upperBallSensorThresholdValue;
  int steadyToLower=STEADY_TO_LOWER;
  int lowerToUpper=LOWER_TO_UPPER;
  enum {offState, onState} state = offState;
  int i;
  int value, sample, timeout;
  int toggleCount=0;
  unsigned char averagePtr=0;
  int averageTable[16];

	init();
  CLRWDT();

  // Signal that inits are done
  // Also signal on on the switch line for the IOBoard
  // 1 pulse, 200ms
  LED = LED_ON;
  SENSOR_SWITCH_OUT_PIN = 0;
  _delay_ms(200);
  CLRWDT();

  LED = LED_OFF;
  SENSOR_SWITCH_OUT_PIN = 1;
  _delay_ms(100);
  CLRWDT();
  
  steadyStateValue = calibrate();
  for(i=0; i<16; i++){
    averageTable[i]=steadyStateValue;
  }
	lowerBallSensorThresholdValue = steadyStateValue + steadyToLower;
  upperBallSensorThresholdValue = lowerBallSensorThresholdValue + lowerToUpper;
  initInterrupt();

  while (true)
  {
    CLRWDT();
    value = getValue();
		switch (state)
		{
			case offState:
                // Pick one sample every 5000
                // Only perform live calibration in the Idle (off) state
                if(++sample>=5000){
                    LED = LED_ON;
                    sample=0;
                    // detects unstability by counting activations between live-calibration
                    if(toggleCount>10){
                        // if switch detected as unstable, reduce the sensitivity
                        steadyToLower = steadyToLower - 5;
                        lowerToUpper = lowerToUpper + 5;
                    }
                    toggleCount=0;
                    // Keep 16 samples in memory
                    averageTable[averagePtr++]=value;
                    if(averagePtr>=16){
                        averagePtr=0;
                    }
                    cumulativeVal=0;
                    // Compute steadyState when new sample arrives
                    // Perform the average over the 16 samples
                    for(i=0; i<16; i++){
                        cumulativeVal+=averageTable[i];
                    }
                    steadyStateValue=cumulativeVal/16;
                    lowerBallSensorThresholdValue = steadyStateValue + steadyToLower;
                    upperBallSensorThresholdValue = lowerBallSensorThresholdValue + lowerToUpper;
                    LED = LED_OFF;
                }
                
				if ( value < lowerBallSensorThresholdValue )
				{
                    SENSOR_SWITCH_OUT_PIN = 0;
                    // Reset the timeout when a ball is detected
                    timeout=0;
                    toggleCount++;
                    LED = LED_ON;
                    _delay_ms(10);
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
                _delay_ms(10);
                
                // Ball Detection timeout (prevents switch inter-lock in on state))
                // If the switch detects for more than 30s, re-do calibration
                // Theory dictates it should be 10ms * 3000 = 30sec, although 6000 gives about 25s...
                if(++timeout>=6000){
                    timeout=0;
                    steadyStateValue = value;
                    for(i=0; i<16; i++){
                      averageTable[i]=steadyStateValue;
                    }
                      lowerBallSensorThresholdValue = steadyStateValue + steadyToLower;
                    upperBallSensorThresholdValue = lowerBallSensorThresholdValue + lowerToUpper;
                }
                
			break;
            
		} //end switch
  } //end while
}

