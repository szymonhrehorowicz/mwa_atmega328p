/*
 * lcd.h
 *
 * Created: 1/17/2024 11:06:43 PM
 *  Author: Szymon
 */ 


#ifndef LCD_H_
#define LCD_H_

/*
    INCLUDES
*/

#include <avr/io.h>

/*
    DEFINES
*/

#define LCD_OK    0u
#define LCD_ERROR 1u

/*
    TYPE DEFINITIONS
*/

typedef uint8_t lcdStatus_t;

/*
    FUNCTIONS
*/

void lcdInit(void);
void lcdWriteCmd(uint8_t cmd);
void lcdWriteData(uint8_t data);
void lcdClear(void);
void lcdHome(void);
void lcdGoto(uint8_t x, uint8_t y);
void lcdWriteString(char *str);
int  lcdPrintf(const __memx char *format, ...);
lcdStatus_t lcdWriteInt(int num);

/*
    EXTERNS
*/

extern volatile lcdStatus_t lcdStatus;


#endif /* LCD_H_ */