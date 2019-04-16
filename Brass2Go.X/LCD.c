#include <xc.h>
#include <pic.h>
#include "LCD.h"
#include "config.h"

void LCD_Init(void) {
    
    LCD_SELECT();
    TRISC5 = 0; //Set RC5 to digital output
    delay(50);
    
    LCD_CMD_MODE();
    LCD_Write(0b00111000);//Function set
    
    LCD_Write(0b00001111);//Display ON

    LCD_Write(0b00000001); //Clear display
    delay(15);
    
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
    delay(1);
}

void LCD_Cmd(char c) {
    LCD_DESELECT();     // turns out you gotta cycle the CS
    LCD_SELECT();
    LCD_CMD_MODE();
    SSP1BUF = c;
    delay(2);
    LCD_DATA_MODE();
}

void LCD_Print(char* str) {
    while (*str) {
        switch(*str) {
            case '\n':
                LCD_Cmd(LCD_NEWLINE);
                break;
            case '\r':
                LCD_Cmd(LCD_HOME);
                break;
            case '\b':
                LCD_Cmd(LCD_SHL);
                break;
            default:
                LCD_Write(*str);
        }
        ++str;
    }
}