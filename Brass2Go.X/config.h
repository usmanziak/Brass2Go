#ifndef B2G_CONFIG_H
#define	B2G_CONFIG_H

#define _XTAL_FREQ 32000000
#pragma config FOSC = INTOSC    // Oscillator Selection Bits->INTOSC oscillator: I/O function on CLKIN pin
#pragma config WDTE = OFF    // Watchdog Timer Enable->WDT disabled
#pragma config PLLEN = ON    // Phase Lock Loop enable->4x PLL is always enabled

#include <stdbool.h>
#include <xc.h>
#include <pic.h>

#define delay(ms) __delay_ms(ms)

// Error defs
typedef enum {
    OPEN_BLOCK,
    RIFF_HEADER,
    WAVE_HEADER,
    FMT_HEADER,
    CODEC,
    BIT_DEPTH,
    DATA_HEADER,
    CHANNELS, 
    CARD_OUT
} Error;

Error global_error;

void error(Error e);

#endif	/* CONFIGURATION_H */

