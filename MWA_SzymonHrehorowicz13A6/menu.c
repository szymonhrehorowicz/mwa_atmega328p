/*
 * menu.c
 *
 * Created: 1/18/2024 10:54:01 AM
 *  Author: Szymon
 */ 

/*
    INCLUDES
*/

#include <avr/pgmspace.h>
#include <stddef.h>
#include <string.h>
#include "menu.h"
#include "lcd.h"

/*
    DEFINES
*/

#define MENU_ROWS 2u

/*
    GLOBAL VARIABLES
*/

static const __flash menuItem_t *currentMenu_p = &menu;

static uint8_t menuIdx;
static uint8_t menuFirstPosition;

/*
    FUNCTIONS
*/

static uint8_t menuGetItemsNo(void)
{
    const __flash menuItem_t *tempMenuItem = currentMenu_p;
    uint8_t idx = 0;
    
    while(tempMenuItem)
    {
        tempMenuItem = tempMenuItem->next;
        idx++;
    }
    
    return idx;
}

static const __flash menuItem_t *menuGetMenuItem(uint8_t idx)
{
    const __flash menuItem_t *tempMenuItem = currentMenu_p;
    
    while((tempMenuItem) && (idx > 0))
    {
        tempMenuItem = tempMenuItem->next;
        idx--;
    }
    
    return tempMenuItem;
}

void menuDisplay()
{
    const __flash menuItem_t *tempMenuItem = menuGetMenuItem(menuFirstPosition);
    
    uint8_t noOfItems = menuGetItemsNo();
    
    lcdClear();
    
    for(uint8_t i = 0; i < MENU_ROWS; i++)
    {
        lcdGoto(0,i);
        
        if(menuIdx == (menuFirstPosition + i) % noOfItems)
        {
            lcdPrintf(">");
        }else
        {
            lcdPrintf(" ");
        }
        
        lcdPrintf(tempMenuItem->label);
        
        tempMenuItem = tempMenuItem->next;
        if(tempMenuItem == NULL)
        {
            if(menuGetItemsNo() > MENU_ROWS)
            {
                tempMenuItem = currentMenu_p;
            }else
            {
                break;
            }
        }
    }
}

void menuNext()
{
    uint8_t no = menuGetItemsNo();
    menuIdx++;
    
    if(no > MENU_ROWS)
    {
        uint8_t distance;
        
        if(menuIdx < menuFirstPosition)
        {
            distance = no - menuFirstPosition + menuIdx;
        }else
        {
            distance = menuIdx - menuFirstPosition;
        }
        
        if(distance >= MENU_ROWS)
        {
            menuFirstPosition++;
        }
    }
    
    menuIdx           %= no;
    menuFirstPosition %= no;
    
    menuDisplay();
}

void menuPrev()
{
    if(menuIdx > 0)
    {
        if(menuIdx == menuFirstPosition)
        {
            menuFirstPosition--;
        }
        menuIdx--;
    }else
    {
        if(menuFirstPosition == 0)
        {
            menuIdx = menuGetItemsNo() - 1;
            if(menuGetItemsNo() > MENU_ROWS)
            {
                menuFirstPosition = menuIdx;
            }
        }else
        {
            menuIdx = menuGetItemsNo() - 1;
        }
    }
    
    menuDisplay();
}

void menuEnter()
{
    const __flash menuItem_t *tempMenuItem = menuGetMenuItem(menuIdx);
    const __flash menuItem_t *submenu      = tempMenuItem->submenu;
    
    menuAction action = tempMenuItem->action;
    
    if(action)
    {
        (*action)();
    }
    
    if(submenu)
    {
        currentMenu_p     = submenu;
        menuIdx           = 0;
        menuFirstPosition = 0;
    }
    
    menuDisplay();
}

void menuBack()
{
    menuFirstPosition = 0;
    menuIdx           = 0;
    currentMenu_p     = currentMenu_p->parent;
}