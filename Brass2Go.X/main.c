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

bool wasPaused = false;

//FATFS filesys;

void error(Error e) {
    // tell someone something went wrong
    global_error = e;
    __nop();
}

void init() {
    // Set the system clock speed to 32MHz and wait for the ready flag.
    OSCCON = 0xF4;
    while(OSCSTATbits.HFIOFR == 0); // wait for clock to settle
    
    ANSB1  = 0;
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
    LCD_DESELECT();
    SD_DESELECT();
    SPI_Init();
    BrassButtons_Init();
    LCD_Init();
    DAC_Init();
    

    TRISA7 = 1; //CD interrupt pin
    IOCAN7 = 1; //detect a falling edge on RA7
    IOCIF = 0;
    IOCAF = 0;
    IOCIE = 1;
    INTCONbits.GIE = 1;
    
    SD_Init(); // TODO: change to fatfs stuff
    
//    while(pf_mount(&filesys) != FR_OK);
}

void __interrupt() isr(void) {          // modifies buffer_read_index
    if (IOCAF7) {
        // CD INTERRUPT
        INTCONbits.GIE = 0; // disable all further interrupts
        card_removed();
    } else {
        // AUDIO INTERRUPT 
        TMR2IF = 0;
        unsigned short level = lbuffer[buffer_read_index++];
        DAC5REFH = (level & 0xff00) >> 8;
        DAC5REFL = (level & 0x00C0) << 8;
        DAC5LD = 1;
        if (buffer_read_index >= BUFFER_SIZE) buffer_read_index = 0;
    }
}

void main(void) {
    INTCONbits.GIE = 0;
    init();
    address = 0;
    
    TRISA6 = 0;
//    TRISA7 = 0;
    
    LCD_SELECT();
    LCD_DATA_MODE();
    task_startScreen();
    LCD_Cmd(LCD_CLS);
    LCD_DATA_MODE();
    LCD_Print("Playing!");
    LCD_DESELECT();
    
    SD_SELECT();
    openFile(address);
    if(channels != 2) samplePending = false;
    timer_Init(sampRate);
    
    task = task_playing;
    while(1) {      // main super loop, will execute the function that is pointed to
                    // by task
        task();
    }
}


//============================================================================//
//============================SUPERLOOP TASKS=================================//

void task_startScreen()
{
    
    LCD_Print("Brass2Go! Press\nstart to play");
    while(VALVE1 == 0);
    delay(500);
}

void task_playing() 
{

    while(1)
    {
                    

        if (blockIndex >= 512) { // end of block condition

            DAC_INT(0);

            // Check for end of the file

            if (byteCounter >= dataLength) {
                PIE1bits.TMR2IE = 0;
                SD_CloseStream();
                SD_DESELECT();
                task = task_analysis;
                return;
            }


            // Read 4 CRC bytes at the end of the block
            SPI_POKE();
            SPI_POKE();
            SPI_POKE();
            SPI_POKE();
            
            DAC_INT(1);
            
            check_buttons = true;
            ++address;
            blockIndex = 0;
            
            
            // Check for 0 -> 1 transition of the pause button
            current_pause = PAUSEBUTTON;
            if(current_pause && !previous_pause) {
                task = task_paused;
                return;
            }
            previous_pause = current_pause;

        } else {
            DAC_INT(0);        // disable timer interrupts while accessing buffer_read_index
            if (buffer_write_index != buffer_read_index) { // read into the buffer if there's space
                DAC_INT(1);

                if (channels == 1) {
                    LATA6 = 1;
                    
                    // Read 16 bit sample into sdata_lo/hi
                    SPI_READ(sdata_lo);
                    SPI_READ(sdata_hi);

                    //Write to the buffer
                    lbuffer[ buffer_write_index ] =  ((sdata_hi << 8) | sdata_lo) - 0x8000;

//                    LATA6 = 0;
                    
                    byteCounter += 2;
                    blockIndex += 2;

                    //CHECK IF THE CORRECT BUTTONS ARE PRESSED AND 
                    //ADD TO THE NUMBER OF ERRORS IF IT IS WRONG
                    
                    if(check_buttons) {
                        check_buttons = false;
                        first_byte = sdata_lo;
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
                } else { // channels !=  1 e.g. file is stereo
                            DAC_INT(0);
                            SD_DESELECT();

                            LCD_Cmd(LCD_CLS);
                            LCD_Print("File is not mono");
                            while(1)
                            {
                                // RESET
                                if(PORTBbits.RB1 == 1)
                                {
                                    delay(250);
                                    LCD_Cmd(LCD_CLS);
                                    RESET();
                                }   
                            }
                    }// end if(channels == 1) -- else
                if (++buffer_write_index >= BUFFER_SIZE) buffer_write_index = 0;
            } else DAC_INT(1);
        }
    }

}

void task_paused()
{ 
    
    //Disable SD Card SPI interface and enable LCD
    DAC_INT(0);
    SD_CloseStream();
    
    SD_DESELECT();

    LCD_Cmd(LCD_CLS);
    LCD_Print("Paused");
    do {
        previous_pause = current_pause;
        current_pause = PAUSEBUTTON;
        
        //RESET 
        
        if(VALVE1)
        {
            delay(250);
            LCD_Cmd(LCD_CLS);
            RESET();
        }
    } while(!(!previous_pause && current_pause));
    previous_pause = true;  // prevent a new pause from being triggered
    
    LCD_Cmd(LCD_CLS); //Clear display
    LCD_Print("Playing!");
    LCD_DESELECT();
    
    SD_SELECT();
    
//     Re-open SD card at the last address
    SD_OpenStream(address);
    //Reset buffer
    buffer_read_index = 0;
    buffer_write_index = 1;
    
    task = task_playing;
//    delay(750);// Delay playback so user can get ready to play the next note
    wasPaused = true;
}

void task_analysis() 
{
    LCD_Cmd(LCD_CLS);
    char message[34];
    sprintf(message, "%d/%d wrong\n%.2f%% correct", number_of_errors, total_presses, 100*(float)(total_presses-number_of_errors)/total_presses);
    LCD_Print(message);
    
    while(!PORTBbits.RB1);
    
    // Reset
    delay(250);
    LCD_Cmd(LCD_CLS);
    RESET();
}

void card_removed() {
    INTCONbits.GIE = 0;     // disable all interrupts
    
    DAC5REFH = 0;   //set the DAC to zero to avoid a weird whine
    DAC5REFL = 0;
    DAC5LD = 1;
    
    delay(5);
    SD_DESELECT();
    LCD_Cmd(LCD_CLS);
    LCD_Print("NO CARD DETECTED\nPlease insert SD");

    delay(500);
    while(!SD_CD_PIN);
    RESET();
}