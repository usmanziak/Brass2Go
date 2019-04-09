#include <pic16f1778.h>
#include "LCD_Drivers.h"
#include "Lab3_Config.h"
#include "Lab3_SPI.h"
#include <xc.h>
#include <pic.h>

void LCD_Init(void) {
    
    LCD_Select();
    TRISC5 = 0; //Set RC5 to digital output
    __delay_ms(50);
    
    LCD_CMD_MODE();
    LCD_Write(0b00111000);//Function set
    
    LCD_Write(0b00001111);//Display ON

    LCD_Write(0b00000001); //Clear display
    __delay_ms(15);
    
    LCD_Write(0b00000110);//Entry mode
    
    // create a new character (cross))
   /*
    RS_PIN = 0;
    LCD_Write(0xb01000000); // set CGRAM address
    
//    RS_PIN = 1;
    LCD_Write(0b00000100); // set this row
    
//    RS_PIN = 0;
//    LCD_Write(0x31); // set CGRAM address
    
//    RS_PIN = 1;
    LCD_Write(0b00000100); // set this row
    
//    RS_PIN = 0;
//    LCD_Write(0x32); // set CGRAM address
//    
//    RS_PIN = 1;
    LCD_Write(0b00000100); // set this row
    
//    RS_PIN = 0;
//    LCD_Write(0x33); // set CGRAM address
//    
//    RS_PIN = 1;
    LCD_Write(0b00011111); // set this row
    
//    RS_PIN = 0;
//    LCD_Write(0x34); // set CGRAM address
//    
//    RS_PIN = 1;
    LCD_Write(0b00000100); // set this row
    
//    RS_PIN = 0;
//    LCD_Write(0x35); // set CGRAM address
//    
//    RS_PIN = 1;
    LCD_Write(0b00000100); // set this row
    
//    RS_PIN = 0;
//    LCD_Write(0x36); // set CGRAM address to
//    
//    RS_PIN = 1;
    LCD_Write(0b00000100); // set this row
    
//    RS_PIN = 0;
//    LCD_Write(0x37); // set CGRAM address to
//    
//    RS_PIN = 1;
    LCD_Write(0b00000100); // set this row
    
    RS_PIN = 0;
    LCD_Write(0x10000000); // set DDRAM address
    */
}

void SD_Select(void) {
    PORTCbits.RC7 = 0;    // SD Chip-Select Enabled
    PORTCbits.RC6 = 1;    // LCD Chip-Select NOT-Enabled
}
void LCD_Select(void) {
   
    PORTCbits.RC7 = 1;    // SD Chip-Select NOT-Enabled
    PORTCbits.RC6 = 0;    // LCD Chip-Select Enabled
    
}

void LCD_Write(char c) {
    SSP1BUF = c;
    __delay_ms(1);
}

void LCD_Print(char* str) {
    while (*str) LCD_Write(*(str++));
}