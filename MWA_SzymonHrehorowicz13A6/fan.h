/*
 * fan.h
 *
 * Created: 1/9/2024 12:42:42 PM
 *  Author: Szymon
 */ 


#ifndef FAN_H_
#define FAN_H_

#include <avr/io.h>

/*
    DEFINES
*/

// STATUS
#define FAN_OK    0u
#define FAN_ERROR 1u

// DIRECTION
#define FAN_DIR_CW  0u // clockwise
#define FAN_DIR_CCW 1u // counterclockwise

// MODES
#define FAN_OFF 0u
#define FAN_ON  1u

/*
    TYPE DEFINITIONS
*/

typedef uint8_t fanStatus_t;
typedef uint8_t fanDirection_t;
typedef uint8_t fanMode_t;

typedef struct
{
    volatile uint8_t *port;
    volatile uint8_t *ddr;
             uint8_t pin;
} controls_t;

typedef struct
{
    fanMode_t       mode;
    uint8_t           speed; // 0 - 100 %
    fanDirection_t direction;
    controls_t       controls;
} fanHandler_t;



/*
    FUNCTIONS
*/

fanStatus_t fanInit(fanHandler_t *fan);
fanStatus_t fanSetSpeed(fanHandler_t *fan, uint8_t speed);
fanStatus_t fanSetDirection(fanHandler_t *fan, fanDirection_t dir);
fanStatus_t fanStart(fanHandler_t *fan);
fanStatus_t fanStop(fanHandler_t *fan);
uint8_t     fanGetMode(fanHandler_t *fan);
uint16_t    fanGetSpeed(fanHandler_t *fan);
uint8_t     fanGetDirection(fanHandler_t *fan);
fanStatus_t fanToggleDirection(fanHandler_t *fan);


/*
    EXTERNS
*/

extern          fanHandler_t fan;
extern volatile fanStatus_t  fanStatus;

#endif /* FAN_H_ */