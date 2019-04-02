#ifndef B2G_DAC_H
#define B2G_DAC_H

#include "wave.h"

typedef struct {
    char b[4];
}Str;

//void DAC_Write_16(SampleFrame sample, unsigned short chans);
void DAC_Write_16(SampleFrame *sample, unsigned short chans);

void DAC_Write_8(unsigned char level);

void DAC_Init(void);

#endif