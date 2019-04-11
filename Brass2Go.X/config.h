#ifndef B2G_CONFIG_H
#define	B2G_CONFIG_H

#define _XTAL_FREQ 32000000
#pragma config FOSC = INTOSC    // Oscillator Selection Bits->INTOSC oscillator: I/O function on CLKIN pin
#pragma config WDTE = OFF    // Watchdog Timer Enable->WDT disabled
#pragma config PLLEN = ON    // Phase Lock Loop enable->4x PLL is always enabled

#include <stdbool.h>
#include <xc.h>

#define delay(ms) __delay_ms(ms)

#endif	/* CONFIGURATION_H */

