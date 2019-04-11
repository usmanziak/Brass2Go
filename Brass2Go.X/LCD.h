#ifndef B2G_LCD_H
#define	B2G_LCD_H

#define LCD_RS_PIN LATC5
#define LCD_CS_PIN LATC6

#define LCD_CMD_MODE() LCD_RS_PIN = 0
#define LCD_DATA_MODE() LCD_RS_PIN = 1

#define LCD_CLS 0x01
#define LCD_HOME 0x02
#define LCD_SHL 0x10
#define LCD_SHR 0x14


void LCD_Init(void);
void LCD_Write(char c);
void LCD_Print(char* str);

#define LCD_DESELECT(){           \
    LCD_CS_PIN = 1;              \
    SSP1CON1bits.WCOL = 0;          \
    }
#define LCD_SELECT(){            \
    LCD_CS_PIN = 0;              \
    SSP1CON1bits.WCOL = 0;          \
    }

#endif	/* LCD_DRIVERS_H */

