#include "interrupt.h"
#include "hardware.h"
#include "main.h"

bool RGBReadytoSet = false;
unsigned long noOfSlaveBits = 0;

void initInterrupt( void )
{
  // Enable Timer 0
  OPTION_REGbits.T0CS = 0;
  OPTION_REGbits.PS = 0;

	// set RA0 Pin to interrupt
	IOCAPbits.IOCAP0 = 1;		// enable interrupt on rising RA0
	IOCANbits.IOCAN0 = 0;		// disable interrupt on falling RA0
	IOCAFbits.IOCAF0 = 0;	  // clear interrupt flags for RA0
	INTCONbits.IOCIF = 0;
	INTCONbits.IOCIE = 1;		// enable interrupt on change interrupt
	INTCONbits.GIE = 1;		// Global interrupt enable

  INTCONbits.PEIE = 1;		// enable the peripheral interrupt
}


unsigned char red, green, blue;
bit d;
int count = 0;

static void interrupt
isr(void)	// Here is the interrupt function - the name is unimportant.
{
//  SPARE1_LATCH = 1;
  if (INTCONbits.IOCIF)
	{
		if (IOCAFbits.IOCAF0)
		{
      d = DATA_IN_PIN;
      if ( count < 24 )
      {
        if (count > 15)
          blue = (blue << 1) | d;
        else if (count > 7)
          green = (green << 1) | d;
        else
          red = (red << 1) | d;
      }
      else
      {
        if (count < noOfSlaveBits)
        {
          DC_DATA_OUT_LATCH = 1;
          if (!d)
            DC_DATA_OUT_LATCH = 0;
        }
        else
        {
          DATA_OUT_LATCH = 1;
          if (!d)
            DATA_OUT_LATCH = 0;
        }
      }
      count++;
    }
    if (count == 24)
    {
      RGBReadytoSet = true;
    }

    INTCONbits.TMR0IE = 1;      // Enable the timer interrupt flag
    INTCONbits.TMR0IF = 0;			// clear the timer interrupt flag
                                // so that it starts a timeout for if there
                                // is no more interrupt
    TMR0 = 0x00;                // start at count 0

    IOCAFbits.IOCAF0 = 0;
		INTCONbits.IOCIF = 0;
  }
  DC_DATA_OUT_LATCH = 0;
  DATA_OUT_LATCH = 0;

  // if the interrupt occurs then there has been no edge interrupt for a while
  // so reset the count, to wait for the next transfer.
  if(INTCONbits.TMR0IF)
	{	// timer interrupt
    count = 0;
    INTCONbits.TMR0IE = 0;    // stop the timer interrupt
	}
//  SPARE1_LATCH = 0;
}
