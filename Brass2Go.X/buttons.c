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
    TRISB = 0b00001110;
    PORTB = 0;
    
    
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
    return PORTB >> 1;
    

}
