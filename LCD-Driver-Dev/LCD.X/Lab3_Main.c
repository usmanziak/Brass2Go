#include <pic.h>
#include "Lab3_Config.h"
#include "Lab3_SPI.h"
#include "Lab3_SD.h"
#include "LCD_Drivers.h"

void main(void) {

    // Set the system clock speed to 32MHz and wait for the ready flag.
    OSCCON = 0xF4;
    while(OSCSTATbits.HFIOFR == 0);

    //Initialize all required peripherals.
    SPI_Init();
    SD_Init();
    LCD_Init();
    
    while(1)
    {
       
      
        
        
        
    }
        
    return;
}

