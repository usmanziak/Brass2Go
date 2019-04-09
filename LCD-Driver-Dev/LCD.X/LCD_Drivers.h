/* 
 * File:   LCD_Drivers.h
 * Author: lukeg
 *
 * Created on April 4, 2019, 10:22 AM
 */

#ifndef LCD_DRIVERS_H
#define	LCD_DRIVERS_H

#define RS_PIN PORTCbits.RC5
#define LCD_CMD_MODE() RS_PIN = 0
#define LCD_DATA_MODE() RS_PIN = 1

#define LCD_CLS 0x01
#define LCD_HOME 0x02
#define LCD_SHL 0x10
#define LCD_SHR 0x14


void LCD_Init(void);
void SD_Select(void);

void LCD_Select(void);
void LCD_Write(char c);
void LCD_Print(char* str);

#define LCD_SET_CS_HIGH(){           \
    PORTCbits.RC6 = 1;              \
    SSP1CON1bits.WCOL = 0;          \
    }
#define LCD_SET_CS_LOW(){            \
    PORTCbits.RC6 = 0;              \
    SSP1CON1bits.WCOL = 0;          \
    }

#endif	/* LCD_DRIVERS_H */

