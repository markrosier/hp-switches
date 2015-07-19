/* 
 * File:   hardware.h
 * Author: markr_000
 *
 * Created on 24 December 2012, 17:59
 */

#ifndef HARDWARE_H
#define	HARDWARE_H

#ifdef	__cplusplus
extern "C" {
#endif

#define false 0
#define true 1
typedef unsigned char bool;

#define OUTPUT_PIN				0
#define INPUT_PIN				1

#define DIGITAL_PIN				0
#define ANALOGUE_PIN				1

#define ADC_GO_NOT_DONE	0b00000010
#define ADC_ENABLE 0b00000001

// Watchdog to 0b01000 = 256ms
// Watchdog to 0b01001 = 512ms
#define WDT_PERIOD  0b01001


// pin 2
#define LED					LATAbits.LATA5
#define LED_DIR 				TRISAbits.TRISA5
#define LED_ON                                  0
#define LED_OFF                                 1

// pin 3
#define BALL_DETECT                   PORTAbits.RA4
#define BALL_DETECT_LATCH             LATAbits.LATA4
#define BALL_DETECT_DIR               TRISAbits.TRISA4
#define BALL_DETECT_PIN_TYPE          ANSELAbits.ANSA4
#define BALL_DETECT_CHANNEL						3
//#define BALL_DETECT_PERCENT_CHANGE		12	// percent

#define HIGH_STEADY_STATE_LEVEL       1200  // steps
#define LOW_STEADY_STATE_LEVEL        300  // steps
#define STEADY_STATE_LEVEL_VARIATION	10  //steps
//#define HYSTERYSIS_PERCENT						8 // percent
#define STEADY_TO_LOWER                 40
#define LOWER_TO_UPPER                  30

// pin 4
#define READ_NOTWRITE_PIN             PORTAbits.RA3
#define READ_NOTWRITE_PIN_DIR         TRISAbits.TRISA3
#define READ_CYCLE                    1
#define WRITE_CYCLE                   0

// pin 5
#define DATA_IN_PIN                   PORTAbits.RA2
#define DATA_IN_LATCH                 LATAbits.LATA2
#define DATA_IN_PIN_DIR               TRISAbits.TRISA2
#define DATA_IN_PIN_TYPE              ANSELAbits.ANSA2

// pin 6
#define DATA_OUT_PIN                  PORTAbits.RA1
#define DATA_OUT_LATCH                LATAbits.LATA1
#define DATA_OUT_PIN_DIR              TRISAbits.TRISA1
#define DATA_OUT_PIN_TYPE             ANSELAbits.ANSA1

// pin 7
#define SENSOR_SWITCH_OUT_PIN 				LATAbits.LATA0
#define SENSOR_SWITCH_OUT_PIN_DIR			TRISAbits.TRISA0
#define SENSOR_SWITCH_OUT_PIN_TYPE    ANSELAbits.ANSA0


#ifdef	__cplusplus
}
#endif

#endif	/* HARDWARE_H */

