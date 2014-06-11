/* 
 * File:   hardware.h
 * Author: markr_000
 *
 * Created on 24 December 2012, 17:59
 */

#ifndef HARDWARE_H
#define	HARDWARE_H

#include <pic16f1824.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define false                       0
#define true                        !false
typedef unsigned char bool;

#define OUTPUT_PIN				          0
#define INPUT_PIN				            1
#define DISABLE_OUTPUT	          	1
#define ENABLE_OUTPUT		          	0


#define DIGITAL_PIN			            0
#define ANALOGUE_PIN		          	1

#define ADC_GO_NOT_DONE	            0b00000010
#define ADC_ENABLE                  0b00000001


// pin 1
#define LED					                LATAbits.LATA5
#define LED_DIR 				            TRISAbits.TRISA5
#define LED_ON                      0
#define LED_OFF                     1

// pin 7
#define BALL_DETECT				          PORTCbits.RC2
#define BALL_DETECT_LATCH		      	LATCbits.LATC2
#define BALL_DETECT_DIR             TRISCbits.TRISC2
#define BALL_DETECT_PIN_TYPE        ANSELCbits.ANSC2
#define BALL_DETECT_CHANNEL		      6
#define BALL_DETECT_PERCENT_CHANGE	40	// percent

#define HIGH_STEADY_STATE_LEVEL		  1000  // steps
#define LOW_STEADY_STATE_LEVEL		  300  // steps
#define STEADY_STATE_LEVEL_VARIATION	10  //steps		
#define HYSTERYSIS_PERCENT		        35 // percent

// Pin 10
#define DATA_IN_PIN                 PORTAbits.RA2
#define DATA_IN_LATCH               LATAbits.LATA2
#define DATA_IN_PIN_DIR             TRISAbits.TRISA2
#define DATA_IN_PIN_TYPE            ANSELAbits.ANSA2

// pin 9
#define DATA_OUT_PIN                PORTCbits.RC0
#define DATA_OUT_LATCH		          LATCbits.LATC0
#define DATA_OUT_PIN_DIR				    TRISCbits.TRISC0
#define DATA_OUT_PIN_TYPE   		    ANSELCbits.ANSC0

// pin 2
#define SENSOR_SWITCH_OUT_PIN 			LATAbits.LATA4
#define SENSOR_SWITCH_OUT_PIN_DIR		TRISAbits.TRISA4
#define SENSOR_SWITCH_OUT_PIN_TYPE  ANSELAbits.ANSA4

#define PWM_LED_ON					        1
#define PWM_LED_OFF					        0

// pin 6
#define RED_PWM_PIN					        PORTCbits.RC3
#define RED_PWM_LATCH				        LATCbits.LATC3
#define RED_PWM_PIN_DIR				      TRISCbits.TRISC3
#define RED_PWM_PIN_TYPE			      ANSELCbits.ANSC3

// pin 8
#define GREEN_PWM_PIN               PORTCbits.RC1
#define GREEN_PWM_LATCH             LATCbits.LATC1
#define GREEN_PWM_PIN_DIR           TRISCbits.TRISC1
#define GREEN_PWM_PIN_TYPE          ANSELCbits.ANSC1

// pin 4
#define BLUE_PWM_PIN					      PORTCbits.RC5
#define BLUE_PWM_LATCH				      LATCbits.LATC5
#define BLUE_PWM_PIN_DIR				    TRISCbits.TRISC5

// pin 5
#define RC4_NOT_USED_PIN			      PORTCbits.RC4
#define RC4_NOT_USED_LATCH		      LATCbits.LATC4
#define RC4_NOT_USED_PIN_DIR  	    TRISCbits.TRISC4


#ifdef	__cplusplus
}
#endif

#endif	/* HARDWARE_H */

