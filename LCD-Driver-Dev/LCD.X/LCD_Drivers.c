#include <pic16f1778.h>
#include "LCD_Drivers.h"

#include "Lab3_SPI.h"
#include <pic.h>
void LCD_Init(void)
{
    
    LCD_Select();
    
    
    
    
    for(int i=0; i<10000; i++);

    TRISC5 = 0; //Set RC5 to digital output
    PORTCbits.RC5 = 0; // RS = 0
    SPI_Write(0b00111000);//Function set
    for(int i=0; i<10000; i++);
   
    
    SPI_Write(0b00001111);//Display ON
    for(int i=0; i<10000; i++);

    SPI_Write(0b1); //Clear display
    for(int i =0; i<10000; i++);
    SPI_Write(0b00000110);//Entry mode
    for(int i =0; i<10000; i++);
    PORTCbits.RC5 = 1; // RS = 1
    for(int i =0; i<10000; i++);

    SPI_Write('A');
}

void SD_Select(void)
{
    PORTCbits.RC7 = 0;    // SD Chip-Select Enabled
    PORTCbits.RC6 = 1;    // LCD Chip-Select NOT-Enabled
}
void LCD_Select(void)
{
   
    PORTCbits.RC7 = 1;    // SD Chip-Select NOT-Enabled
    PORTCbits.RC6 = 0;    // LCD Chip-Select Enabled
    
}
void LCD_ClearScreen(void)
{
    LCD_Select();
    SPI_Write(1);

}
void LCD_ReturnHome(void)
{
    LCD_Select();
    SPI_Write(0x2);

}
void LCD_ShiftCursorLeft(void)
{
    
    LCD_Select();
    SPI_Write(0x10);

    
}
void LCD_ShiftCursorRight(void)
{
    LCD_Select();
    SPI_Write(0x14);
    
}