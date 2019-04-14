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
#define ONRED   LATBbits.LATB5 = 1;
#define OFFRED  LATBbits.LATB5 = 0;
#define ONGREEN LATBbits.LATB4 = 1;
#define OFFGREEN LATBbits.LATB4 = 0;

#define PAUSEBUTTON    PORTBbits.RB0
#endif


