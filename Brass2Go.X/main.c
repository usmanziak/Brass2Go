#include <pic.h>
#include <xc.h>
#include <stdio.h>
#include "config.h"
#include "timer.h"
#include "SPI.h"
#include "SD.h"
#include "DAC.h"
#include "wave.h"
#include "error.h"
#include "buttons.h"

#include "LCD.h"

//#pragma config FOSC = ECH
//#pragma config CLKOUTEN = ON

#define BUFFER_SIZE 32
#define READINTO(s) readBytes((char*)&s, sizeof(s));

#define ON0     LATCbits.LATC0 = 1;
#define OFF0    LATCbits.LATC0 = 0;
#define ON1     LATCbits.LATC1 = 1;
#define OFF1    LATCbits.LATC1 = 0;
#define ON6     LATBbits.LATB0 = 1;
#define OFF6    LATBbits.LATB0 = 0;

unsigned short channels;
unsigned long  sampRate;
unsigned short bitsPerSamp;

unsigned long byteCounter = 0;

char sdata[2];

//SampleFrame sdata;

short lbuffer[BUFFER_SIZE];
short rbuffer[BUFFER_SIZE];
unsigned short buffer_read_index = 0;
unsigned short buffer_write_index = 1;

short blockIndex = 0;
unsigned long dataLength;
char readMessage = 0xFF;

BlockState state = CLOSED;
long address = 0;
bool isPlaying = true;
bool samplePending = false;

void (*task)();

////////////////////////////////////////////////////////////////////////////////

inline void pulse() {          // send a test pulse to LATC0;
    LATCbits.LATC0 = 1;
    LATCbits.LATC0 = 0;
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
        dataLength = data.ckSize;
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
    TRISC7 = 0;
    
//    LATCbits.LATC6 = 1;
    
    //Initialize all required peripherals.

    SPI_Init();
    BrassButtons_Init();
    SD_Init();
    LCD_Init();    
    DAC_Init();
}

void __interrupt() isr(void) {          // modifies buffer_read_index
    PIR1bits.TMR2IF = 0;
//    ON0
    short level = lbuffer[buffer_read_index++];
    DAC5REFH = (level & 0xff00) >> 8;
    DAC5REFL = (level & 0x00C0) << 8;
    DAC5LD = 1;
    if (buffer_read_index >= BUFFER_SIZE) buffer_read_index = 0;
//    OFF0
}

////////////////////////////////////////////////////////////////////////////////
bool check_buttons = 0;
unsigned char first_byte = 0;
short number_of_errors = 0;
short total_presses = 0;


void task_playing();
void task_analysis();
    
    
void main(void) {
    init();
    address = 0;
    
    LCD_SELECT();
    LCD_DATA_MODE();
    LCD_Print("Playing!\n");
    LCD_DESELECT();
    
    SD_SELECT();
    openFile(address);
    if(channels != 2) samplePending = false;
    timer_Init(sampRate);
    
    task = &task_playing;

    while(1) {      // main loop
        (*task)();
    }
}

void task_analysis() {
    LCD_SELECT();
    LCD_CMD_MODE();
    LCD_Write(LCD_CLS);
    delay(1);
    
    LCD_DATA_MODE();
    char message[34];
    sprintf(message, "%d/%d wrong\n%.2f%% correct", number_of_errors, total_presses, 100*(float)(total_presses-number_of_errors)/total_presses);
    LCD_Print(message);
    
    while(1);
}

void task_playing() {

    if (blockIndex >= 512) { // end of block condition
        if (byteCounter >= dataLength) {
            PIE1bits.TMR2IE = 0;
            SD_CloseStream();
            SD_DESELECT();
            task = &task_analysis;
            return;
        }
        ++address;
        blockIndex = 0;
        SPI_Read();
        SPI_Read();
        SPI_Read();
        SPI_Read();
        check_buttons = true;
        OFF6
//                    ON6
    } else {
        PIE1bits.TMR2IE = 0;        // disable timer interrupts while accessing buffer_read_index
        if (buffer_write_index != buffer_read_index) { // read into the buffer if there's space
            PIE1bits.TMR2IE = 1;
//                        ON1
            switch (channels) {
                case 1:
                    SSP1BUF = 0xFF;
                    while(SSP1STATbits.BF == 0);
                    sdata[0] = SSP1BUF;
                    
                    SSP1BUF = 0xFF;
                    while(SSP1STATbits.BF == 0);
                    sdata[1] = SSP1BUF;

                    byteCounter += 2;
                    
                    lbuffer[ buffer_write_index ] = /*rbuffer[ buffer_write_index ] =*/ *((short*)sdata) - 0x8000;
                    ++buffer_write_index;

                    //CHECK IF THE CORRECT BUTTONS ARE PRESSED AND 
                    //ADD TO THE NUMBER OF ERRORS IF IT IS WRONG
                    
                    if(check_buttons) {
                        check_buttons = false;
                        first_byte = sdata[0];
                        if(first_byte % 2 == 1){        //IF BIT 0 == 1
                            ++total_presses;
                            ON6
                            if(Check_Buttons(first_byte) == 0)
                                ++number_of_errors;
                        }
                    }
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