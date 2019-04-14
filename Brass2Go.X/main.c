#include <pic.h>
#include <xc.h>
#include <stdio.h>
#include "main.h"
#include "config.h"
#include "timer.h"
#include "SPI.h"
#include "SD.h"
#include "DAC.h"
#include "wave.h"
#include "error.h"
#include "buttons.h"
#include "LCD.h"

unsigned char sdata_lo;
unsigned char sdata_hi;

bool check_buttons = 0;
unsigned char first_byte = 0;
short number_of_errors = 0;
short total_presses = 0;

bool previous_pause;
bool current_pause;

void init() {
    // Set the system clock speed to 32MHz and wait for the ready flag.
    OSCCON = 0xF4;
    while(OSCSTATbits.HFIOFR == 0); // wait for clock to settle
    
    ANSB1 = 0;
    TRISB4 = 0; //GREEN LED
    TRISB5 = 0; //RED LED
    TRISC0 = 0;
    TRISC1 = 0;
    TRISB0 = 0;
    TRISC6 = 0;
    TRISC7 = 0;
    TRISB0 = 1;
    
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
    if (blockIndex == 510) {
        __nop();
    }
    LATA7 = 1;
    unsigned short level = lbuffer[buffer_read_index++];
    DAC5REFH = (level & 0xff00) >> 8;
    DAC5REFL = (level & 0x00C0) << 8;
    DAC5LD = 1;
    if (buffer_read_index >= BUFFER_SIZE) buffer_read_index = 0;
    LATA7 = 0;
}

void main(void) {
    init();
    address = 0;
    
    TRISA6 = 0;
    TRISA7 = 0;
    
    LCD_SELECT();
    LCD_DATA_MODE();
    task_startScreen();
    LCD_CMD_MODE();
    
    LCD_Write(LCD_CLS); //Clear display
    __delay_ms(15);
    LCD_DATA_MODE();
    LCD_Print("Playing!");
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
    
    while(!PORTBbits.RB1);
    delay(1000);
    RESET();
}

void task_playing() {

    while(1)
    {
        

        if (blockIndex >= 512) { // end of block condition

            DAC_INT(0);
            SSP1BUF = 0xFF;
            if (byteCounter >= dataLength) {
                PIE1bits.TMR2IE = 0;
                SD_CloseStream();
                SD_DESELECT();
                task = &task_analysis;
                return;
            }
            while(SSP1STATbits.BF == 0);
            SSP1BUF = 0xFF;
            ++address;
            blockIndex = 0;
            while(SSP1STATbits.BF == 0);
            SSP1BUF = 0xFF;
            
            check_buttons = true;
            while(SSP1STATbits.BF == 0);
            SSP1BUF = 0xFF;
            while(SSP1STATbits.BF == 0);
            DAC_INT(1);
            
            current_pause = PAUSEBUTTON;
            if(current_pause && !previous_pause) {
                task = &task_paused;
                return;
            }
            previous_pause = current_pause;

        } else {
            DAC_INT(0);        // disable timer interrupts while accessing buffer_read_index
            if (buffer_write_index != buffer_read_index) { // read into the buffer if there's space
                DAC_INT(1);

                if (channels == 1) {
                    LATA6 = 1;
                    SSP1BUF = 0xFF;
                    while(SSP1STATbits.BF == 0);
                    sdata_lo = SSP1BUF;

                    SSP1BUF = 0xFF;
                    while(SSP1STATbits.BF == 0);
                    sdata_hi = SSP1BUF;


                    lbuffer[ buffer_write_index ] =  ((sdata_hi << 8) | sdata_lo) - 0x8000;

//                    LATA6 = 0;

                    byteCounter += 2;
                    blockIndex += 2;

                    //CHECK IF THE CORRECT BUTTONS ARE PRESSED AND 
                    //ADD TO THE NUMBER OF ERRORS IF IT IS WRONG

                    if(check_buttons) {
                        check_buttons = false;
                        first_byte = sdata[0];
                        if(first_byte % 2 == 1){        //IF BIT 0 == 1

                            // note is to be played here, so check which buttons are depressed 
                            ++total_presses;


                            if(!Check_Buttons(first_byte))
                            {
                                // The buttons were wrong

                                ONRED
                                OFFGREEN
                                ++number_of_errors;
                            }else
                            {
                                OFFRED
                                ONGREEN
                            }

                        }
                    }
                } else {
//                    if (samplePending) {
////                            samplePending = false;
//                        SSP1BUF = 0xFF;
//
//                        LATA6 = 1;
//                        while(SSP1STATbits.BF == 0);
//                        unsigned char sdata_lo = SSP1BUF;
//
//                        SSP1BUF = 0xFF;
//                        while(SSP1STATbits.BF == 0);
//                        unsigned char sdata_hi = SSP1BUF;
//
//
//                        lbuffer[ buffer_write_index ] =  ((sdata_hi << 8) | sdata_lo) - 0x8000;
//
//                                                    while(SSP1STATbits.BF == 0);
//                        unsigned char sdata_lo = SSP1BUF;
//
//                        SSP1BUF = 0xFF;
//                        while(SSP1STATbits.BF == 0);
//                        unsigned char sdata_hi = SSP1BUF;
//                        lbuffer[ buffer_write_index ] =  ((sdata_hi << 8) | sdata_lo) - 0x8000;
//
//
//                                                    LATA6 = 0;
//                        byteCounter += 2;
//                        ++buffer_write_index;
//                    } else {
//                        samplePending = true;
//                        sdata[0] = SPI_Read();
//                        sdata[1] = SPI_Read();
//                        lbuffer[ buffer_write_index ] = *((short*)sdata) - 0x8000;
//                    }



                    SSP1BUF = 0xFF;
                    while(SSP1STATbits.BF == 0);
                    sdata_lo = SSP1BUF;

                    SSP1BUF = 0xFF;
                    while(SSP1STATbits.BF == 0);
                    sdata_hi = SSP1BUF;

                    lbuffer[ buffer_write_index ] =  ((sdata_hi << 8) | sdata_lo) - 0x8000;

                    SSP1BUF = 0xFF;
                    while(SSP1STATbits.BF == 0);
                    sdata_lo = SSP1BUF;

                    SSP1BUF = 0xFF;
                    while(SSP1STATbits.BF == 0);
                    sdata_hi = SSP1BUF;

                    rbuffer[ buffer_write_index ] =  ((sdata_hi << 8) | sdata_lo) - 0x8000;



                    byteCounter += 4;
                    blockIndex += 4;
                }
                if (++buffer_write_index >= BUFFER_SIZE) buffer_write_index = 0;
            } else DAC_INT(1);
        }
    }

}
void task_startScreen()
{
    LCD_Print("Brass2Go! Press\nStart to play");
    while(PORTBbits.RB1 == 0);
    delay(500);
    return;
    
}

void task_paused()
{ 
    DAC_INT(0);
    
    SD_DESELECT();
    LCD_SELECT();
    LCD_CMD_MODE();
    LCD_Write(LCD_CLS); //Clear display
    delay(5);
    LCD_DATA_MODE();
    LCD_Print("Paused");
    
    /* FIXME:
     Once LCD_Write() is called, the SD card gives a weird value for every 256th sample, giving noise after playing again
     */
    
    do {
        previous_pause = current_pause;
        current_pause = PAUSEBUTTON;
    } while(!(!previous_pause && current_pause));
    previous_pause = true;
    
    LCD_CMD_MODE();
    LCD_Write(LCD_CLS); //Clear display
    delay(5);
    LCD_DATA_MODE();
    LCD_Print("Playing!");
    LCD_DESELECT();
    
    SD_SELECT();
    task = &task_playing;
    DAC_INT(1);
}