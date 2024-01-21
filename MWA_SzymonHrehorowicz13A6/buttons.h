/*
 * buttons.h
 *
 * Created: 1/20/2024 3:42:08 PM
 *  Author: Szymon
 */ 


#ifndef BUTTONS_H_
#define BUTTONS_H_

/*
    INCLUDES
*/

#include <avr/io.h>

/*
    DEFINES
*/

#define BTN_NEXT  PC2
#define BTN_ENTER PC3

#define BUTTONS_COUNTER_LIMIT 512u

/*
    FUNCTIONS
*/

void buttonsInit(void);
void buttonsIntHandler(void);
void checkButtons(void);
uint8_t buttonNextClicked(void);
void buttonNextClear(void);
uint8_t buttonEnterClicked(void);
void buttonEnterClear(void);

/*
    EXTERNS
*/

extern volatile uint8_t buttons;

#endif /* BUTTONS_H_ */