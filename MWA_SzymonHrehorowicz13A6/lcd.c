/*
 * lcd.c
 *
 * Created: 1/17/2024 11:06:36 PM
 *  Author: Szymon
 */ 

/*
    INCLUDES
*/

#ifndef F_CPU
    #define F_CPU 11059200UL
#endif

#include <stdarg.h>
#include <stdio.h>
#include <util/delay.h>
#include <stdlib.h>
#include "lcd.h"
#include "uartControl.h"

/*
    DEFINES
*/

#define LCD_MAX_INT_LENGTH 4u

#define LCD_EN_DDR  DDRC
#define LCD_EN_PORT PORTC
#define LCD_EN_PIN  PC1

#define LCD_RS_DDR  DDRC
#define LCD_RS_PORT PORTC
#define LCD_RS_PIN  PC0

#define LCD_DATA_DDR  DDRB
#define LCD_DATA_PORT PORTB

#define LCD_CMD  LCD_RS_PORT &= ~(1 << LCD_RS_PIN)
#define LCD_DATA LCD_RS_PORT |=  (1 << LCD_RS_PIN)

#define LCD_SET_EN   LCD_EN_PORT |= (1 << LCD_EN_PIN)
#define LCD_CLEAR_EN LCD_EN_PORT &= ~(1 << LCD_EN_PIN)

#define LCD_PUT_TO_BUFFER(data) LCD_DATA_PORT = (LCD_DATA_PORT & 0xE1) | ((data << 1) & 0x1E)

/*
    FUNCTIONS
*/

static void lcdSendNibble(uint8_t nibble)
{
    LCD_SET_EN;
    LCD_PUT_TO_BUFFER(nibble);
    _delay_ms(1);
    LCD_CLEAR_EN;
    _delay_ms(1);
}

static void lcdSendByte(uint8_t byte)
{
    lcdSendNibble(byte >> 4);
    lcdSendNibble(byte);
}

void lcdWriteCmd(uint8_t cmd)
{
    LCD_CMD;
    lcdSendByte(cmd);
    _delay_ms(2);
}

void lcdWriteData(uint8_t data)
{
    LCD_DATA;
    lcdSendByte(data);
    _delay_ms(2);
}

void lcdInit(void)
{
    LCD_EN_DDR   |= (1 << LCD_EN_PIN);
    LCD_RS_DDR   |= (1 << LCD_RS_PIN);
    LCD_DATA_DDR |= (0x0F << 1);
    
    LCD_CMD;
    
    _delay_ms(5);
    lcdSendNibble(3);
    _delay_ms(1);
    lcdSendNibble(3);
    _delay_ms(1);
    lcdSendNibble(3);
    _delay_ms(1);
    lcdSendNibble(2);
    _delay_ms(1);
    
    lcdWriteCmd(0x28); // 4-bit, 2 lines
    _delay_ms(1);
    lcdWriteCmd(0x01); // clear display
    _delay_ms(1);
    lcdWriteCmd(0x06); // increment DDRAM, no shift
    _delay_ms(1);
    lcdWriteCmd(0x0C); // turn display on, no cursor and no blinking
    _delay_ms(1);
    
    lcdPrintf("Szymon");
    lcdGoto(0,1);
    lcdPrintf("Hrehorowicz");
    _delay_ms(3000);
}

void lcdClear(void)
{
    lcdWriteCmd(0x01);
    lcdWriteCmd(0x02);
}

void lcdHome(void)
{
    lcdWriteCmd(0x02);
    _delay_ms(2);
}

void lcdGoto(uint8_t x, uint8_t y)
{
    lcdWriteCmd(128 | ((y == 1) ? 64 : 0) | x);
}

void lcdWriteString(char *str)
{
    uint16_t i = 0;
    
    while(str[i] && i < 1000)
    {
        lcdWriteData(str[i]);
        i++;
    }
}

lcdStatus_t lcdWriteInt(int num)
{
    lcdStatus_t rslt = LCD_OK;
    uint8_t str[LCD_MAX_INT_LENGTH];
    uint8_t len;
    
    if(num > 9999)
    {
        rslt = LCD_ERROR;
        return rslt;
    }
    
    intToString(num, str, &len);

    for(uint8_t i = 0; i < len; i++)
    {
        lcdWriteData(str[i]);
    }
    
    return rslt;
}

int lcdPrintf(const char *format, ...)
{
    va_list argptr;
    char str[40];
    int n;
    
    va_start(argptr, format);
    
    n = sprintf(str, format, argptr);
    
    lcdWriteString(str);
    va_end(argptr);
    
    return n;
}