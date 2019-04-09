#include <pic.h>
#include "Lab3_Config.h"
#include "Lab3_SPI.h"
#include "Lab3_SD.h"
#include "LCD_Drivers.h"

void main(void) {
    TRISCbits.TRISC6 = 0;
    TRISCbits.TRISC7 = 0;
    
    // Set the system clock speed to 32MHz and wait for the ready flag.
    OSCCON = 0xF4;
    while(OSCSTATbits.HFIOFR == 0);

    //Initialize all required peripherals.
    SPI_Init();
    SD_Init();
    LCD_Init();
    
//    RS_PIN = 0;
//    LCD_Write(0x80 | 0x00); // set DDRAM address
    
    LCD_DATA_MODE();
    LCD_Print("Hello World!\n");
    LCD_Print("This was\b\b\bis the 2nd line");
    
    while(1);
    return;
}

