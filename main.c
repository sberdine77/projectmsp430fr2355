#include <msp430.h> 
#include <stateManager.h>
#include <serialReader.h>

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	setupTimer();
    setupUart();

    while(1) {
        switch(state){
            case WAVE:
                generateWave();
                nextState();
                break;
            case LOGGER:
                //
                nextState();
                break;
            case SERIALREADER:
                readUart();
                nextState();
                break;
            case LEDWAVE:
                blink();
                nextState();
                break;
            default:
                state = WAVE;
                break;
        }
    }


	return 0;
}
