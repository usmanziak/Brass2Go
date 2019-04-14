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
//    ON0
    short level = lbuffer[buffer_read_index++];
    DAC5REFH = (level & 0xff00) >> 8;
    DAC5REFL = (level & 0x00C0) << 8;
    DAC5LD = 1;
    if (buffer_read_index >= BUFFER_SIZE) buffer_read_index = 0;
//    OFF0
}

bool check_buttons = 0;
unsigned char first_byte = 0;
short number_of_errors = 0;
short total_presses = 0;


void main(void) {
    init();
    address = 0;
    
    LCD_SELECT();
    LCD_DATA_MODE();
    task_startScreen();
    LCD_CMD_MODE();
    
    LCD_Write(0b00000001); //Clear display
    __delay_ms(15);
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
    
    while(!PORTBbits.RB1);
    delay(1000);
    RESET();
}

void task_playing() {

    while(1)
    {
        
        if(PAUSEBUTTON)
        {
            task = &task_paused;
            return;
        }
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

        } else {
            DAC_INT(0);        // disable timer interrupts while accessing buffer_read_index
            if (buffer_write_index != buffer_read_index) { // read into the buffer if there's space
                DAC_INT(1);

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
                        ;
                }


                buffer_write_index %= BUFFER_SIZE;
                blockIndex += 2;

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
    delay(500);
    while(PORTBbits.RB0 == 0);
    delay(500);
    
    

    /* TODO: Release from pause state if button goes from 0 to 1
     * 
        while(!(current_pause && !previous_pause)) {
            previous_pause = current_pause;
            current_pause = PAUSEBUTTON;
        } 

    */
    task = &task_playing;
    DAC_INT(1);
    return;
    
    
}