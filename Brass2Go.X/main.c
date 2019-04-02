#include <pic.h>
#include <xc.h>
#include "config.h"
#include "timer.h"
#include "SPI.h"
#include "SD.h"
#include "DAC.h"
#include "wave.h"
#include "error.h"

#define BUFFER_SIZE 32
#define READINTO(s) readBytes((char*)&s, sizeof(s));

#define ON     PORTCbits.RC0 = 1;
#define OFF    PORTCbits.RC0 = 0;

unsigned short channels;
unsigned long sampRate;
unsigned short bitsPerSamp;

char sdata[2];
//SampleFrame sdata;

short buffer[BUFFER_SIZE];
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

void init() {
    // Set the system clock speed to 32MHz and wait for the ready flag.
    OSCCON = 0xF4;
    while(OSCSTATbits.HFIOFR == 0); // wait for clock to settle
    
    TRISBbits.TRISB1 = 1;
    ANSELBbits.ANSB1 = 0;
    
    TRISCbits.TRISC0 = 0;
    TRISCbits.TRISC1 = 0;
    
    //Initialize all required peripherals.
    SPI_Init();
    SD_Init();
    DAC_Init();
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
    SD_OpenBlock(a);
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

void __interrupt() isr(void) {          // modifies buffer_read_index
    PIR1bits.TMR2IF = 0;
//    ON
    
    short level = buffer[buffer_read_index++] - 0x8000;  // add offset to make unsigned
    DAC1REFH = (level & 0xff00) >> 8;
    DAC1REFL = (level & 0x00C0) << 8;
    DAC1LD = 1;
//    pulse();
    if (buffer_read_index >= BUFFER_SIZE) buffer_read_index = 0;
//    OFF
}

void main(void) {

    init();
    while(1) { // play the whole song again
        
        address = 0;
        
        openFile(address);
        __nop();
        if(channels != 2) samplePending = false;
        timer_Init(sampRate);
        
        while(1) {
            
            if (state == CLOSED && isPlaying) {
                
                SD_OpenBlock(address);
                state = OPENING;

                readMessage = 0xFF;
            }

                //We don't know when the SD card will start sending data, but the first byte
            //is always 0xFE. Read bytes until the response contains at least one zero.
            if (state == OPENING) {
                readMessage = SPI_Read();
                if (readMessage != 0xFF) {
                    if (readMessage == 0xFE){
                        state = OPEN;
                    } else {
                        state = CLOSED;
                        error(OPEN_BLOCK);
                    }
                }
            }

            if (state == OPEN) {
                if (blockIndex >= 512) { // end of block condition // format specific
                    SD_CloseBlock();
                    ++address;
                    blockIndex = 0;
                    state = CLOSED;
                } else if (buffer_write_index != buffer_read_index) { // read into the buffer if there's space
                    
//                    switch (channels) {
//                        case 1:
//                            sdata.bytes[0] = SPI_Read();
//                            sdata.bytes[1] = SPI_Read();
//                            buffer[ buffer_write_index++ ].mono16 = sdata.mono16;
//                            __nop();
//                        break;
//                        case 2:
//                            if (samplePending) {
//                                sdata.bytes[2] = SPI_Read();
//                                sdata.bytes[3] = SPI_Read();
//                                buffer[ buffer_write_index++ ].stereo16 = sdata.stereo16;
//                                samplePending = false;
//                            } else {
//                                sdata.bytes[0] = SPI_Read();
//                                sdata.bytes[1] = SPI_Read();
//                                samplePending = true;
//                            }
//                        break;
//                        default:
//                            error(CHANNELS);
//                    }
                    ON
                    sdata[0] = SPI_Read();
                    sdata[1] = SPI_Read();
                    
                    buffer[ buffer_write_index++ ] = *((short*)sdata);
                    OFF
//                      sdata[0] = SPI_Read();
//                      sdata[1] = SPI_Read();
                    
//                      buffer[ buffer_write_index++ ] = *((short*)sdata);
                    
                    if (buffer_write_index >= BUFFER_SIZE) buffer_write_index = 0;
                    blockIndex += 2;

                }
            }
            
        }
//        while(!PORTBbits.RB1);
    }
        
    return;
}