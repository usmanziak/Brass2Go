/* 
 * File:   LCD_Drivers.h
 * Author: lukeg
 *
 * Created on April 4, 2019, 10:22 AM
 */

#ifndef LCD_DRIVERS_H
#define	LCD_DRIVERS_H


void LCD_Init(void);
void SD_Select(void);

void LCD_Select(void);
void LCD_ClearScreen(void);
void LCD_ReturnHome(void);
void LCD_ShiftCursorLeft(void);
void LCD_ShiftCursorRight(void);

#endif	/* LCD_DRIVERS_H */

