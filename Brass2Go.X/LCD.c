#include <xc.h>
#include <pic.h>
#include "LCD.h"
#include "config.h"

void LCD_Init(void) {
    
    LCD_SELECT();
    TRISC5 = 0; //Set RC5 to digital output
    __delay_ms(50);
    
    LCD_CMD_MODE();
    LCD_Write(0b00111000);//Function set
    
    LCD_Write(0b00001111);//Display ON

    LCD_Write(0b00000001); //Clear display
    __delay_ms(15);
    
    LCD_Write(0b00000110);//Entry mode
    
    LCD_DESELECT();
}
    
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
//}

void LCD_Write(char c) {
    SSP1BUF = c;
    __delay_ms(1);
}

void LCD_Print(char* str) {
    while (*str) {
        switch(*str) {
            case '\n':
                LCD_CMD_MODE();
                LCD_Write(0x80 | 0x40);
                LCD_DATA_MODE();
                break;
            case '\r':
                LCD_CMD_MODE();
                LCD_Write(LCD_HOME);
                LCD_DATA_MODE();
                break;
            case '\b':
                LCD_CMD_MODE();
                LCD_Write(LCD_SHL);
                LCD_DATA_MODE();
                break;
            default:
                LCD_Write(*str);
        }
        ++str;
    }
}