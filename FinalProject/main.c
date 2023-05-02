#include <stateManager.c>
//#include <waveGenerator.c>
#include <serialReader.c>


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	setupTimmer();
	setupUart();
	
	while(1) {
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
            blink();
            break;
        default:
            //
            break;
        }
	}

	return 0;
}
