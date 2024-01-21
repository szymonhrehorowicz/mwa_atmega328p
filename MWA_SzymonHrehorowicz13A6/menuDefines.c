/*
 * menuDefines.c
 *
 * Created: 1/18/2024 11:20:55 AM
 *  Author: Szymon
 */ 

/*
 * menudef.c
 *
 * Created: 1/18/2024 10:58:05 AM
 *  Author: Szymon Hrehorowicz
 */

/*
    INCLUDES
*/

#ifndef F_CPU
    #define F_CPU 11059200UL
#endif

#include <util/delay.h>
#include <avr/pgmspace.h>
#include "menu.h"
#include "lcd.h"
#include "fan.h"
#include "adc.h"
#include "buttons.h"

/*
    DEFINES
*/

#define PGM_STR(X) ((const __flash char[]) { X })

/*
    GLOBAL VARIABLES
*/

fanHandler_t         fan;
volatile fanStatus_t fanStatus;
volatile lcdStatus_t lcdStatus;
volatile uint8_t     buttons;

/*
    FUNCTIONS 1
*/

void menuStartStopFan();
void menuSetNewSpeed();
void menuChangeDirection();
void menuGetCurrentSpeed();

/*
    GLOBAL VARIABLE DEFINITIONS
*/

menuItem_t const __flash menu;
menuItem_t const __flash speedMenuCurrentSpeed;

menuItem_t const __flash speedMenuReturn       = {PGM_STR("<return>"), menuBack, &speedMenuCurrentSpeed, 0, 0};
menuItem_t const __flash speedMenuSetSpeed     = {PGM_STR("Set new speed"), menuSetNewSpeed, &speedMenuCurrentSpeed, 0, &speedMenuReturn};
menuItem_t const __flash speedMenuCurrentSpeed = {PGM_STR("Current speed"), menuGetCurrentSpeed, &menu, 0, &speedMenuSetSpeed};

menuItem_t const __flash directionMenu = {PGM_STR("Change dir"), menuChangeDirection, &menu, 0, 0};
menuItem_t const __flash speedMenu     = {PGM_STR("Change speed"), 0, &menu, &speedMenuCurrentSpeed, &directionMenu};

menuItem_t const __flash menu = {PGM_STR("START/STOP"), menuStartStopFan, 0, 0, &speedMenu};

/*
    FUNCTIONS 2
*/

void menuStartStopFan()
{
    uint8_t fanMode = fanGetMode(&fan);
    
    if(fanMode == FAN_ON)
    {
        fanStatus = fanStop(&fan);
    }else
    {
        fanStatus = fanStart(&fan);
    }
}

void menuGetCurrentSpeed()
{
    lcdClear();
    lcdStatus = lcdWriteInt(fanGetSpeed(&fan));
    lcdPrintf(" \%");
    _delay_ms(2000);
}

void menuSetNewSpeed()
{
    _delay_ms(200);
    
    adcOn();
    uint16_t newSpeed = 0;
    
    while(!buttonEnterClicked())
    {
        newSpeed = adcMap(adcMeasure(), 0, 100);
        lcdClear();
        lcdStatus = lcdWriteInt(newSpeed);
        _delay_ms(100);
    }
    
    buttonEnterClear();
    
    fanStatus = fanSetSpeed(&fan, newSpeed);
}

void menuChangeDirection()
{
    fanStatus = fanToggleDirection(&fan);
}