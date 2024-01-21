/*
 * main.c
 *
 * Created: 1/17/2024 8:02:21 PM
 *  Author: Szymon Hrehorowicz
 */ 

/*
    INCLUDES
*/

#define  F_CPU 11059200UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>
#include <stdbool.h>
#include "fan.h"
#include "lcd.h"
#include "menu.h"
#include "buttons.h"
#include "uart.h"
#include "uartControl.h"

/*
    DEFINES
*/

#define ERROR_LED      PC4

/*
    GLOBAL VARIABLES
*/

volatile fanStatus_t  fanStatus;
volatile lcdStatus_t  lcdStatus;
volatile uint8_t      buttons;
volatile uint8_t      statusRequest;
volatile uint8_t      cmdToExec;
volatile uint8_t      cmdLength;
volatile uint8_t      cmdBuff[MAX_CMD_LENGTH];
         fanHandler_t fan;
         dataQueue_t  dataQueueStart;
         uint8_t      cmdError;

/*
    FUNCTIONS
*/

void checkForErrors(void);

int main(void)
{
    DDRC  |=  (1 << ERROR_LED);
    PORTC &= ~(1 << ERROR_LED);
    
    sei();
    
    fanStatus = fanInit(&fan);
    lcdInit();
    adcInit();
    uartInit();
    buttonsInit();
    dataQueueInit();
    menuDisplay();
    
    dataQueue_t *currentDataElement = &dataQueueStart;
    statusRequest = 0;

    while(1)
    {    
        checkButtons();
        checkForErrors();
        
        if(statusRequest)
        {
            currentDataElement->action();
            currentDataElement = currentDataElement->next;
        }
        
        if(cmdToExec)
        {
            translateCmd();
        }
    }
}

void checkForErrors(void)
{
    if((fanStatus == FAN_ERROR) || (lcdStatus == LCD_ERROR) || cmdError)
    {
        PORTC |=  (1 << ERROR_LED);
    }else
    {
        PORTC &= ~(1 << ERROR_LED);
    }
}


/*
    INTERRUPT HANDLERS
*/

// Interrupts for fan control
ISR(TIMER0_OVF_vect)
{
    *(fan.controls.port) |= (1<<fan.controls.pin);
}

ISR(TIMER0_COMPA_vect)
{
    *(fan.controls.port) &= ~(1<<fan.controls.pin);
}

// Interrupt for buttons control
ISR(TIMER1_COMPA_vect)
{
    buttonsIntHandler();
}

// Interrupt for UART
ISR(USART_RX_vect)
{
    commandIntHandler();
}
