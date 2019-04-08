#include <pic.h>
#include <xc.h>
#include "config.h"
#include "timer.h"
#include "SPI.h"
#include "SD.h"
#include "DAC.h"
#include "wave.h"
#include "error.h"
#include "buttons.h"

//#pragma config FOSC = ECH
//#pragma config CLKOUTEN = ON

#define BUFFER_SIZE 32
#define READINTO(s) readBytes((char*)&s, sizeof(s));

#define ON0     PORTCbits.RC0 = 1;
#define OFF0    PORTCbits.RC0 = 0;
#define ON1     PORTCbits.RC1 = 1;
#define OFF1    PORTCbits.RC1 = 0;
#define ON6     PORTBbits.RB0 = 1;
#define OFF6    PORTBbits.RB0 = 0;

unsigned short channels;
unsigned long  sampRate;
unsigned short bitsPerSamp;

char sdata[2];

//SampleFrame sdata;

short lbuffer[BUFFER_SIZE];
short rbuffer[BUFFER_SIZE];
unsigned short buffer_read_index = 0;
unsigned short buffer_write_index = 1;

short blockIndex = 0;
unsigned long filesize;
char readMessage = 0xFF;

BlockState state = CLOSED;
long address = 0;
bool isPlaying = true;
bool samplePending = false;

////////////////////////////////////////////////////////////////////////////////

inline void pulse() {          // send a test pulse to RC0;
    PORTCbits.RC0 = 1;
    PORTCbits.RC0 = 0;
}

bool fourCCeq(FourCC a, FourCC b) {
    for (char i=0; i<4; i++) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

inline void readBytes(char* dest, int len) {
    for (int i=0; i<len; i++) {
        dest[i] = SPI_Read();
        ++blockIndex;
        if (blockIndex >= 512) { // end of block condition // format specific
            SD_CloseBlock();
            ++address;
            blockIndex = 0;
            state = CLOSED;
        }
    }
    
}

void openFile(long a) {
    SD_OpenStream(a);
    state = OPEN;
    char response = 0xFF;
    while (response == 0xFF) response = SPI_Read();
    if (response != 0xFE) error(OPEN_BLOCK);
    
    {
        RiffTag riff;
        READINTO(riff);
        if (!fourCCeq(riff.ckID, RIFF_CC)) error(RIFF_HEADER);
    }{
        FmtChunk fmt;
        READINTO(fmt);
        if (!fourCCeq(fmt.WAVEID, WAVE_CC)) error(WAVE_HEADER);
        if (!fourCCeq(fmt.fmtHeader.ckID, FMT_CC)) error(FMT_HEADER);
        if (fmt.formatTag != WAVE_FORMAT_PCM) error(CODEC);
        if (fmt.bitsPerSamp != 16) error(BIT_DEPTH);
        
        channels = fmt.channels;
        sampRate = fmt.sampRate;
        
    }{
        RiffTag data;
        READINTO(data);
        if (!fourCCeq(data.ckID, DATA_CC)) error(DATA_HEADER);
        filesize = data.ckSize;
    }
        __nop();
    
}

void init() {
    // Set the system clock speed to 32MHz and wait for the ready flag.
    OSCCON = 0xF4;
    while(OSCSTATbits.HFIOFR == 0); // wait for clock to settle
    
    TRISB1 = 1;
    ANSB1 = 0;
    
    TRISC0 = 0;
    TRISC1 = 0;
    TRISB0 = 0;
    TRISC6 = 0;
    
    PORTCbits.RC6 = 1;
    
    //Initialize all required peripherals.
    SPI_Init();
    SD_Init();
    DAC_Init();
}

void __interrupt() isr(void) {          // modifies buffer_read_index
    PIR1bits.TMR2IF = 0;
//    ON0
    short level = lbuffer[buffer_read_index++];
    DAC1REFH = (level & 0xff00) >> 8;
    DAC1REFL = (level & 0x00C0) << 8;
    DAC1LD = 1;
    if (buffer_read_index >= BUFFER_SIZE) buffer_read_index = 0;
//    OFF0
}

////////////////////////////////////////////////////////////////////////////////
    char check_buttons = 0;
    unsigned char first_byte = 0;
    char number_of_errors = 0;
    char total_presses = 0;
void main(void) {
        init();
    BrassButtons_Init();
    while(1) { // play the whole song again
        
        address = 0;
        
        openFile(address);
        if(channels != 2) samplePending = false;
        timer_Init(sampRate);
        
        while(1) {
            
//            if (state == CLOSED && isPlaying) {
//                ON6
//                SD_OpenBlock(address);
//                state = OPENING;
//                readMessage = 0xFF;
//                OFF6
//            }
//
//                //We don't know when the SD card will start sending data, but the first byte
//            //is always 0xFE. Read bytes until the response contains at least one zero.
//            if (state == OPENING) {
//                ON6
//                readMessage = SPI_Read();
//                if (readMessage != 0xFF) {
//                    if (readMessage == 0xFE){
//                        state = OPEN;
//                    } else {
//                        state = CLOSED;
//                        error(OPEN_BLOCK);
//                    }
//                    OFF6
//                }
//            }

            if (state == OPEN) {
                if (blockIndex >= 512) { // end of block condition // format specific
//                    SD_CloseBlock();
                    ++address;
                    blockIndex = 0;
                    SPI_Read();
                    SPI_Read();
                    SPI_Read();
                    SPI_Read();
                    check_buttons = 1;
//                    state = CLOSED;
//                    ON6
                } else {
                    PIE1bits.TMR2IE = 0;        // disable timer interrupts while accessing buffer_read_index
                    if (buffer_write_index != buffer_read_index) { // read into the buffer if there's space
                        PIE1bits.TMR2IE = 1;
//                        ON1
                        switch (channels) {
                            case 1:
                                sdata[0] = SPI_Read();
                                sdata[1] = SPI_Read();
                                lbuffer[ buffer_write_index ] = /*rbuffer[ buffer_write_index ] =*/ *((short*)sdata) - 0x8000;
                                ++buffer_write_index;
                                //CHECK IF THE CORRECT BUTTONS ARE PRESSED AND 
                                //ADD TO THE NUMBER OF ERRORS IF IT IS WRONG
                                /*
                                if(check_buttons)
                                {
                                    check_buttons = 0;
                                    first_byte = sdata[0];
                                    if(first_byte % 2 == 1){
                                        total_presses++;

                                        if(Check_Buttons(first_byte) == 0)
                                            number_of_errors++;
                                        
                                    }
                                 */
                            break;
                            case 2:
                                if (samplePending) {
                                    samplePending = false;
                                    sdata[0] = SPI_Read();
                                    sdata[1] = SPI_Read();
                                    rbuffer[ buffer_write_index++ ] = *((short*)sdata) - 0x8000;
                                    __nop();
                                } else {
                                    samplePending = true;
                                    sdata[0] = SPI_Read();
                                    sdata[1] = SPI_Read();
                                    lbuffer[ buffer_write_index ] = *((short*)sdata) - 0x8000;
                                }
                            break;
                            default:
                                error(CHANNELS);
                        }
                        
//                        OFF1

                        buffer_write_index %= BUFFER_SIZE;
                        blockIndex += 2;

                    } else PIE1bits.TMR2IE = 1;
                }
            }
            
        }
//        while(!PORTBbits.RB1);
    }
        
    return;
}