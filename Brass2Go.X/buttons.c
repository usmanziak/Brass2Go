/* 
 * File:   buttons.c
 * Author: Luke Garland
 *
 * Created on March 28, 2019, 4:03 PM
 */
#include <xc.h>
#include "buttons.h"




void BrassButtons_Init(void)
/*  REQUIRES: Nothing
 * 
 *  PROMISES: Sets Pins RB1, RB2, RB3 as digital output
 */
{
    ANSELB = 0;
    
    TRISB1 = 1;
    TRISB2 = 1;
    TRISB3 = 1;
    return;
    
}


unsigned char BrassButtons_Pressed(void)
/*  REQUIRES: Buttons must be intialized with BrassButtons_Init();
 * 
 *  PROMISES: Returns a char with a combination of button presses.
 *            Ex: if 12 is pressed, it will return 0b00000011
 *                if 123 is pressed it will return 0b00000111
 *                if 23 is pressed, it will return 0b00000110
 */
{
    return (0b00001110 & PORTB) >> 1;
}

unsigned char Check_Buttons(unsigned char encoded_byte)
//Returns 1 if the buttons that are currently pressed are the buttons that should be
//Pressed (encoded byte)
{
    unsigned char correct_buttons =  (0b00001110 & encoded_byte) >> 1;
    if(BrassButtons_Pressed() == correct_buttons)
        return 1;
    else return 0;
}