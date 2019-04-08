/* 
 * File:   buttons.h
 * Author: Luke Garland
 *
 * Created on March 28, 2019, 4:02 PM
 */

#ifndef BUTTONS_H
#define	BUTTONS_H

void BrassButtons_Init(void);

unsigned char BrassButtons_Pressed(void);
unsigned char Check_Buttons(unsigned char encoded_byte);
typedef enum {b_0 = 0, b_1 = 1, b_2 = 2, b_3 = 4, b_12 = 3, b_13 = 5, b_23 = 6, b_123 = 7} Buttons;
//Enum for button combinations
#endif


