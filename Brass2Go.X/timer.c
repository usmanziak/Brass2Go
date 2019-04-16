#include "config.h"
#include "timer.h"

// REQUIRES: Nothing
// PROMISES: Set the system clock speed to 32MHz.
//           Configure Timer2 to trigger with the given period in microseconds.
//           Start Timer2. It will automatically restart when it finishes.
int timer_Init(long rate) {
    PIR1bits.TMR2IF = 0;
    PIE1bits.TMR2IE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    
    switch (rate) {
        case 16000: 
            T2PR = 100;             // period 100
            T2CLKCON = 0x03;        // HFINTOSC 16MHz
            T2CON = 0x89;           // prescaler 1:1 postscaler 1:10
            PIR1bits.TMR2IF = 0;
            return 0;
        break;
        case 44100:
            T2PR = 32;             // period 33
            T2CLKCON = 0x03;        // HFINTOSC 16MHz
            T2CON = 0x8A;           // prescaler 1:1 postscaler 1:11.
            return 0;
        break;
        case 48000:
            T2PR = 111;             // period 111
            T2CLKCON = 0x03;        // HFINTOSC 16MHz
            T2CON = 0x82;           // prescaler 1:1 postscaler 1:3.
            return 0;
        break;
        case 32000:
            T2PR = 100;
            T2CLKCON = 0x03;
            T2CON = 0x84; // prescaler 1:1 postscaler 1:5
            return 0;
        break;
        case 22050:
            T2PR = 65;
            T2CLKCON = 0x03;
            T2CON = 0x8A; // prescaler 1:1 postscaler 1:11
            return 0;
        default:
            return 1;
    }
}

    // T2PR: TMR2 PERIOD REGISTER (Page 287)
    // The timer runs at 1MHz, so it increments every 1us.
    
    
    // T2CLKCON: TIMER2 CLOCK SELECTION REGISTER (Page 306))
    // Bits 8:4 = 0b00000   These bits are not implemented
    // Bits 3:0 = 0b0010    Clock source is the system clock
    

    // T2CON: TIMERx CONTROL REGISTER (Page 307)
    // Bit  7   = 0b1       Timer is on.
    // Bits 6:4 = 0b101     Prescaler is 1:32 (Timer increments every 32 cycles)
    // Bits 3:0 = 0b0000    Postscaler is 1:1 
    
            
    // TMR2IF is an interrupt flag, and is set whenever Timer2 expires.
    // We are not using interrupts, but it can still be checked manually.
    // It is cleared here as we start the timer.
    