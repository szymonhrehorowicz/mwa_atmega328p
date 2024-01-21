/*
 * fan.c
 *
 * Created: 1/9/2024 12:42:34 PM
 *  Author: Szymon
 */ 

/*
    INCLUDES
*/

#include "fan.h"

/*
    DEFINES
*/

// PORTS
#define A_PORT PORTB
#define A_DDR  DDRB
#define A_PIN  PB0
#define B_PORT PORTD
#define B_DDR  DDRD
#define B_PIN  PD7

// OCR
#define FAN_OCR_LOWEST 101u
#define FAN_OCR_RANGE  255u

// SPEED
#define FAN_MIN_SPEED 0u
#define FAN_MAX_SPEED 100u

/*
    GLOBAL VARIABLES
*/

fanHandler_t fan;

/*
    FUNCTIONS
*/

fanStatus_t fanInit(fanHandler_t *fan)
{
    fanStatus_t rslt = FAN_OK;
    
    // Set pins as outputs
    A_DDR |= (1 << A_PIN);
    B_DDR |= (1 << B_PIN);
    
    // Turn off all control pins
    A_PORT &= ~(1 << A_PIN);
    B_PORT &= ~(1 << B_PIN);
    
    rslt = fanStop(fan);
    rslt = fanSetSpeed(fan, FAN_MAX_SPEED/2);
    rslt = fanSetDirection(fan, FAN_DIR_CW);
    
    // Timer initialization
    TCCR1A |= (1 << WGM01)  | (1 << WGM00); // FAST PWM
    TIMSK0 |= (1 << OCIE0A) | ( 1<< TOIE0); // Turn on interrupts
    
    return rslt;
}

static fanStatus_t fanSetMode(fanHandler_t *fan, fanMode_t mode)
{
    fanStatus_t rslt = FAN_OK;
    
    if((mode == FAN_OFF) || (mode == FAN_ON))
    {
        fan->mode = mode;
    }else
    {
        rslt = FAN_ERROR;
    }
    
    return rslt;
}

static fanStatus_t fanCalculateOCR(uint8_t speed, uint8_t *value_p)
{
    fanStatus_t rslt = FAN_OK;
    
    if((speed >= 0) && (speed <= FAN_MAX_SPEED))
    {
        *value_p = FAN_OCR_LOWEST +  ((FAN_OCR_RANGE - FAN_OCR_LOWEST) * ((float)speed / (float)FAN_MAX_SPEED));
    }else
    {
        rslt = FAN_ERROR;
    }
    
    return rslt;
}

fanStatus_t fanSetSpeed(fanHandler_t *fan, uint8_t speed)
{
    fanStatus_t rslt = FAN_OK;
    
    fanMode_t tmp_mode = fan->mode;
    
    fanStop(fan);
    
    if((speed >= 1) && (speed <= FAN_MAX_SPEED))
    {
        fan->speed = speed;
    }else if(speed < 1)
    {
        fan->speed = FAN_MIN_SPEED;
        fanStop(fan);
        tmp_mode = FAN_OFF;
    }else
    {
        rslt = FAN_ERROR;
    }
    
    if((tmp_mode == FAN_ON) && (rslt != FAN_ERROR))
    {
        fanStart(fan);
    }
    
    return rslt;
}

fanStatus_t fanSetDirection(fanHandler_t *fan, fanDirection_t dir)
{
    fanStatus_t rslt = FAN_OK;
    
    fanMode_t tmp_mode = fan->mode;
    
    fanStop(fan);
    
    if(dir == FAN_DIR_CW)
    {
        fan->direction = dir;
        
        fan->controls.ddr  = &A_DDR;
        fan->controls.port = &A_PORT;
        fan->controls.pin  = A_PIN;
    }else if(dir == FAN_DIR_CCW)
    {
        fan->direction = dir;
        
        fan->controls.ddr  = &B_DDR;
        fan->controls.port = &B_PORT;
        fan->controls.pin  = B_PIN;
    }else
    {
        rslt = FAN_ERROR;
    }
    
    if((tmp_mode == FAN_ON) && (rslt != FAN_ERROR))
    {
        fanStart(fan);
    }
    
    return rslt;
}

fanStatus_t fanStart(fanHandler_t *fan){
    fanStatus_t rslt = FAN_OK;
    
    rslt = fanSetMode(fan, FAN_ON);
    
    if(fan->speed == FAN_MAX_SPEED)
    {
        *(fan->controls.port) |= (1<<fan->controls.pin);
    }else
    {
        uint8_t newOCR = 0;
        
        fanCalculateOCR(fan->speed, &newOCR);
        
        
        TCNT0 = 0; // Reset Counter
        OCR0A = newOCR;
        TCCR0B |= (1 << CS02); // Start timer
    }
    
    return rslt;
}

fanStatus_t fanStop(fanHandler_t *fan)
{
    fanStatus_t rslt = FAN_OK;
    
    rslt = fanSetMode(fan, FAN_OFF);
    
    TCCR0B &= ~(1 << CS02);  // stop timer
    A_PORT &= ~(1 << A_PIN);
    B_PORT &= ~(1 << B_PIN);

    return rslt;
}

uint8_t fanGetMode(fanHandler_t *fan)
{
    return fan->mode;
}

uint16_t fanGetSpeed(fanHandler_t *fan)
{
    return fan->speed;
}

uint8_t fanGetDirection(fanHandler_t *fan)
{
    return fan->direction;
}

fanStatus_t fanToggleDirection(fanHandler_t *fan)
{
    fanStatus_t rslt = FAN_OK;
    
    if(fan->direction == FAN_DIR_CCW)
    {
        rslt = fanSetDirection(fan, FAN_DIR_CW);
    }else
    {
        rslt = fanSetDirection(fan, FAN_DIR_CCW);
    }
    
    return rslt;
}