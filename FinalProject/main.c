#include <msp430.h>
#include <stateManager.c>
//#include <waveGenerator.c>
#include <serialReader.c>


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	setupUart();
	
	switch(state){
	case WAVE:
	    generateWave();
	    break;
	case LOGGER:
        //
        break;
	case SERIALREADER:
        readUart();
        break;
	case LEDWAVE:
        //
        break;
	case LEDLOGGER:
        //
        break;
	default:
        //
	    break;
	}

	return 0;
}