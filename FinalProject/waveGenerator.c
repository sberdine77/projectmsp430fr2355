/*
 * waveGenerator.c
 *
 *  Created on: May 1, 2023
 *      Author: savioberdine
 */

#include <string.h>

#define RECT 0;
#define TRI 1;
#define SEN 2;

const char* RECTANGULAR_TYPE = "rect";
const char* TRIANGULAR_TYPE = "tri";
const char* SENOIDAL_TYPE = "sen";

int waveType = 0;
int wavePoints = 50;
int drawFlag = 0;

int compareType(const char *string, const char *type) {
    if(strcmp(type, RECTANGULAR_TYPE) == 0) {
        return 1;
    }
    return 0;
}

void changeWaveType(char *newType) {
    if(compareType(newType, RECTANGULAR_TYPE)) {
        wavePoints = 50;
        waveType = RECT;
    } else if(compareType(newType, TRIANGULAR_TYPE)) {
        wavePoints = 50;
        waveType = TRI;
    } else if(compareType(newType, SENOIDAL_TYPE)) {
        wavePoints = 50;
        waveType = SEN;
    }
}

void draw() {
    //TO-DO
    drawFlag = 0;
    wavePoints--;
}

void generateWave() {
    if(!drawFlag) {
        return;
    }
    if(waveType == 0) {
        draw(); //Rectangular
    } else if(waveType == 1) {
        draw(); //Triangular
    } else if(waveType == 2) {
        draw(); //Senoid
    }
}
