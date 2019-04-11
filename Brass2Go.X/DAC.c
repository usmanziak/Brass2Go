/*
 * File:   DAC.c
 * Author: Toshi
 *
 * Created on March 26, 2019, 12:32 AM
 */

#include <xc.h>
#include <pic.h>
#include "config.h"
#include "DAC.h"


void DAC_Write_16( SampleFrame *sample, unsigned short chans) {
    short level = sample->mono16;
    level -= 0x8000; // add offset to make unsigned
    DAC5REFH = (level & 0xff00) >> 8;
    DAC5REFL = (level & 0x00C0) << 8;
    DAC5LD = 1;
    
    // right channel
}

void DAC_Write_8(unsigned char level) {
    DAC5REFH = level;
    DAC5REFL = 0;
    DAC5LD = 1;
}

void DAC_Init() {
    TRISA = 0xFB;
    DAC5CON0 = 0xE0; // left justified
}