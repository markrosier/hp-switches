
#include "interrupt.h"
#include "hardware.h"
#include "main.h"
#include <xc.h>

void initInterrupt( void )
{
	IOCAPbits.IOCAP2 = 1;		// enable interrupt on rising RA2
	IOCANbits.IOCAN2 = 0;		// disable interrupt on falling RA2
	IOCAFbits.IOCAF = 0;		// clear interrupt flags for whole Port
	INTCONbits.IOCIF = 0;
	INTCONbits.IOCIE = 1;		// enable interrupt on change interrupt
	INTCONbits.GIE = 1;     // Global interrupt enable

  INTCONbits.PEIE = 1;		// enable the peripheral interrupt
}

bit d;

static void interrupt
isr(void)	// Here is the interrupt function - the name is unimportant.
{
 if (INTCONbits.IOCIF)
	{
		if (IOCAFbits.IOCAF2)
		{
      d = DATA_IN_PIN;
      DATA_OUT_LATCH = 1;
      if (!d)
      DATA_OUT_LATCH = 0;
    }
    if (d == 1)
      _delay_us(2);
    IOCAFbits.IOCAF2 = 0;
		INTCONbits.IOCIF = 0;
  }
  DATA_OUT_LATCH = 0;
}
