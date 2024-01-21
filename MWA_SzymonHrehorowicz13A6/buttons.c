/*
 * buttons.c
 *
 * Created: 1/20/2024 3:41:56 PM
 *  Author: Szymon
 */ 

/*
    INCLUDES
*/

#include "buttons.h"
#include "menu.h"

/*
    FUNCTIONS
*/

void buttonsInit(void)
{
    DDRC   &= ~(1 << BTN_NEXT) & ~(1 << BTN_ENTER);
    
    OCR1AH |= ((uint16_t)BUTTONS_COUNTER_LIMIT >> 8);
    OCR1AL |= (uint8_t)BUTTONS_COUNTER_LIMIT;
    TIMSK1 |= (1 << OCIE1A);
    TCCR1B |= (1 << CS12) | (1 << CS10);  // Normal mode, 1024 prescaler
}

void buttonsIntHandler(void)
{
    static uint8_t prevButtonsStatus = 0xFF;
    uint8_t currentButtonsStatus = PINC;
    
    if(!(currentButtonsStatus & (1 << BTN_NEXT)) && (prevButtonsStatus & (1 << BTN_NEXT)))
    {
        buttons |= (1 << BTN_NEXT);
    }
    
    if(!(currentButtonsStatus & (1 << BTN_ENTER)) && (prevButtonsStatus & (1 << BTN_ENTER)))
    {
        buttons |= (1 << BTN_ENTER);
    }
    
    prevButtonsStatus = currentButtonsStatus;
}

void checkButtons(void)
{
    if(buttonNextClicked())
    {
        buttonNextClear();
        menuNext();
    }
    
    if(buttonEnterClicked())
    {
        buttonEnterClear();
        menuEnter();
    }
}

uint8_t buttonNextClicked(void)
{
    return buttons & (1 << BTN_NEXT);
}

void buttonNextClear(void)
{
    buttons &= ~(1 << BTN_NEXT);
}

uint8_t buttonEnterClicked(void)
{
    return buttons & (1 << BTN_ENTER);
}

void buttonEnterClear(void)
{
    buttons &= ~(1 << BTN_ENTER);
}
