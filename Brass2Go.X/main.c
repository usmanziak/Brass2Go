#include <pic.h>
#include "config.h"
#include "timer.h"
#include "SPI.h"
#include "SD.h"
#include "DAC.h"

#define BUFFER_SIZE 32

unsigned char a0 = 0;
unsigned char a1 = 0;
unsigned char a2 = 0;
unsigned char a3 = 0;

char sdata[2];

short buffer[BUFFER_SIZE];
char buffer_read_index = 0;
char buffer_write_index = 1;

short blockIndex = 0;

typedef enum {
    OPEN,
    OPENING,
    CLOSED
} BlockState;

BlockState state = CLOSED;
bool isPlaying = true;

////////////////////////////////////////////////////////////////////////////////

void error() {
    // tell someone something went wrong
}

void pulse() {          // send a test pulse to RC0;
    PORTCbits.RC0 = 1;
    PORTCbits.RC0 = 0;
}

void increment_addr() {
    if (a0 == 255) {
        a0 = 0;
        if (a1 == 255) {
            a1 = 0;
            if (a2 == 255) {
                a2 = 0;
                ++a3;
            } else ++a2;
        } else ++a1;
    } else ++a0;
}

void init() {
    // Set the system clock speed to 32MHz and wait for the ready flag.
    OSCCON = 0xF4;
    while(OSCSTATbits.HFIOFR == 0);
    
    TRISBbits.TRISB1 = 1;
    ANSELBbits.ANSB1 = 0;
    
    TRISCbits.TRISC0 = 0;
    TRISCbits.TRISC1 = 0;

    //Initialize all required peripherals.
    SPI_Init();
    SD_Init();
    DAC_Init();
    timer_Init(48000);
}

void main(void) {
    init();
    
    while(1) { // play the whole song again
        
        a0 = 0;     //reset block address
        a1 = 0;
        a2 = 0;
        a3 = 0;

        
        while(1) {
            
            if (state == CLOSED && isPlaying) {
                SD_OpenBlock(a3, a2, a1, a0);
                state = OPENING;
            }

                //We don't know when the SD card will start sending data, but the first byte
            //is always 0xFE. Read bytes until the response contains at least one zero.
            if (state == OPENING) {
                char readMessage = SPI_Read();
                if (readMessage != 0xFF) {
                    if (readMessage == 0xFE){
                        state = OPEN;
                    } else {
                        state = CLOSED;
                        error();
                    }
                }
            }

            if (state == OPEN) {
                if (blockIndex >= 512) { // end of block condition // format specific
                    SD_CloseBlock();
                    increment_addr();
                    blockIndex = 0;
                    state = CLOSED;
                } else if (buffer_write_index != buffer_read_index) { // read into the buffer if there's space
                    sdata[0] = SPI_Read();
                    sdata[1] = SPI_Read();
                    buffer[ buffer_write_index++ ] = *((short*)sdata);
//                    pulse();
                    if (buffer_write_index >= BUFFER_SIZE) buffer_write_index = 0;
                    
                    blockIndex += 2;
                }
            }
            

            
            if (PIR1bits.TMR2IF && isPlaying) {                  // write to DAC
//                static short y = 0;
//                PORTCbits.RC0 = 1;
                DAC_Write_16( buffer[buffer_read_index++] );
//                PORTCbits.RC0 = 0;
                pulse();
                
                if (buffer_read_index >= BUFFER_SIZE) buffer_read_index = 0;
                PIR1bits.TMR2IF = 0;
                PORTCbits.RC1 = 0;
            }
            
        }
//        while(!PORTBbits.RB1);
    }
        
    return;
}