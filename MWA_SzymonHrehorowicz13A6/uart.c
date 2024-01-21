/*
 * uart.c
 *
 * Created: 1/20/2024 4:47:09 PM
 *  Author: Szymon
 */ 

/*
    INCLUDES
*/

#include "uart.h"

/*
    DEFINES
*/

#ifndef F_CPU
    #define  F_CPU 11059200UL
#endif

#define UART_MAX_INT_LENGTH 3u

/*
    FUNCTIONS
*/

static void uart9600()
{
    #define BAUD 9600UL
    #include <util/setbaud.h>
    
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
    
    #if USE_2X
        UCSR0A |= (1 << U2X0);
    #else
        UCSR0A &= ~(1 << U2X0);
    #endif
}

static inline void uartWait()
{
    while(!(UCSR0A & (1 << UDRE0)));
}

void uartInit()
{
    uart9600();
    
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);
}

void uartSendByte(uint8_t byte)
{
    uartWait();
    UDR0 = byte;
}

void uartSendString(const __memx uint8_t *str, uint8_t len)
{
    for(uint8_t i = 0; i < len; i++)
    {
        uartSendByte(str[i]);
    }
}