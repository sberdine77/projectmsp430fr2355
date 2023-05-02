/*
 * stateManager.c
 *
 *  Created on: May 1, 2023
 *      Author: savioberdine
 */

#define WAVE 0
#define LOGGER 1
#define SERIALREADER 2
#define LEDWAVE 3
#define LEDLOGGER 4

int state = 0;

void nextState() {
    if(state == LEDLOGGER) {
        state = 0;
        return;
    }
    state += 1;
}