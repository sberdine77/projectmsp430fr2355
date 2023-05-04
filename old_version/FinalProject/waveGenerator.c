/*
 * waveGenerator.c
 *
 *  Created on: May 1, 2023
 *      Author: savioberdine
 */

#include <string.h>
#include <msp430.h>
#include <timmerConfig.h>

#define RECT 0
#define TRI 1
#define SEN 2

const char* RECTANGULAR_TYPE = "rect";
const char* TRIANGULAR_TYPE = "tri";
const char* SENOIDAL_TYPE = "sen";

int waveType = 0;
int drawCompare = 31;
int passo = 31;

int rectangularWave[32] = {4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,  4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};
int triangularWave[32] = {0,    256, 512,  768, 1024,   1280, 1536,  1792, 2048,    2303, 2559,  2815, 3071,   3327, 3583,  3839, 4095,  3839, 3583,  3327, 0.75,   2815, 2559,  2303, 2048,    1792, 1536,  1280, 1024,   768, 512,  256};
int senWave[32] = {2047, 2460, 2855, 3218, 3533, 3787, 3970, 4073, 4095, 4032, 3888, 3668, 3382, 3042, 2661, 2254, 1840, 1433, 1052, 712, 426, 206, 62, 0, 21, 124, 307, 561, 876, 1239, 1634, 2047};

int compareType(const char *string, const char *type) {
    if(strcmp(type, RECTANGULAR_TYPE) == 0) {
        return 1;
    }
    return 0;
}

void changeWaveType(char *newType) {
    if(compareType(newType, RECTANGULAR_TYPE)) {\
        waveType = RECT;
    } else if(compareType(newType, TRIANGULAR_TYPE)) {
        waveType = TRI;
    } else if(compareType(newType, SENOIDAL_TYPE)) {
        waveType = SEN;
    }
}

void draw() {
    if(waveType == RECT) {
    //Rectangular
    } else if(waveType == TRI) {
    //Triangular
    } else if(waveType == SEN) {
    //Senoid
    }
}

int canDraw() {
    if(mili >= drawCompare) {
        drawCompare += passo;
        return 1;
    } else {
        return 0;
    }
}

void generateWave() {
    if(!canDraw()) {
        return;
    }
    draw();
}


