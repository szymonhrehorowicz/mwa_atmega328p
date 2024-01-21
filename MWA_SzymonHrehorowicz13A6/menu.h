/*
 * menu.h
 *
 * Created: 1/18/2024 10:54:08 AM
 *  Author: Szymon Hrehorowicz
 */ 


#ifndef MENU_H_
#define MENU_H_

/*
    TYPE DEFINITIONS
*/

typedef void (*menuAction)();

typedef struct MenuItem
{
    const __flash char * const     label;
    menuAction                     action;
    const __flash struct MenuItem *parent;
    const __flash struct MenuItem *submenu;
    const __flash struct MenuItem *next;
} menuItem_t;

/*
    FUNCTIONS
*/

void menuDisplay();
void menuNext();
void menuPrev();
void menuEnter();
void menuBack();

/*
    EXTERNS
*/

extern menuItem_t const __flash menu;

#endif /* MENU_H_ */