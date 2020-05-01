/*
 * LCD.h
 *
 *  Created on: 06.12.2017
 *      Author: vimala
 */

#ifndef LCD_H_
#define LCD_H_

#include "driverlib.h"
#include "string.h"

#define	LCD_HEART				   (0x0004)
#define	LCD_TIMER				   (0x0008)

#define NUM_CHAR 6


/* LCD memory locations for each character position on the display */
#define pos1 4   /* Digit A1 - L4  */
#define pos2 6   /* Digit A2 - L6  */
#define pos3 8   /* Digit A3 - L8  */
#define pos4 10  /* Digit A4 - L10 */
#define pos5 2   /* Digit A5 - L2  */
#define pos6 18  /* Digit A6 - L18 */

//***** Global Variables ******************************************************
extern const char digit[10][2];            // Segment values for each number character
extern const char alphabetBig[26][2];      // Segment values for each alphabet character

void LCD_init(void);
void LCD_showChar( char, int );
void LCD_displayNumber( unsigned long );
void displayScrollText(char *);


#endif /* LCD_H_ */
