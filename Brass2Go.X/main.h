/* 
 * File:   main.h
 * Author: luke
 *
 * Created on April 13, 2019, 4:33 PM
 */

#ifndef MAIN_H

#define	MAIN_H


#define DAC_INT(a) PIE1bits.TMR2IE = a // DAC interrupt enable / disable


// Super loop task function prototypes
void task_playing();
void task_analysis();
void task_startScreen();
void task_paused();
void (*task)();

#endif	/* MAIN_H */

