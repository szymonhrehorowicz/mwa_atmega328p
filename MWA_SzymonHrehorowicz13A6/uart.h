/*
 * uart.h
 *
 * Created: 1/20/2024 4:47:16 PM
 *  Author: Szymon
 */ 


#ifndef UART_H_
#define UART_H_

/*
    INCLUDES
*/

#include <avr/io.h>
#include <avr/pgmspace.h>

/*
    FUNCTIONS
*/

void uartInit();
void uartSendByte(uint8_t byte);
void uartSendString(const __memx uint8_t *str, uint8_t len);

#endif /* UART_H_ */