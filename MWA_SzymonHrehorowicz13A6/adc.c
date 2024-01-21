/*
 * adc.c
 *
 * Created: 1/18/2024 2:11:28 PM
 *  Author: Szymon
 */ 

/*
    INCLUDES
*/

#include <avr/io.h>
#include "adc.h"

/*
    DEFINES
*/

#define ADC_RANGE 1024.0f

/*
    FUNCTIONS
*/

void adcInit(void)
{
    ADMUX  |= (1 << REFS0) | (1 << MUX0) | (1 << MUX2);   // Select ref voltage and channel
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Enable ADC, 128 prescaler
}

void adcOff(void)
{
    ADCSRA &= ~(1 << ADEN);
}

void adcOn(void)
{
    ADCSRA |= (1 << ADEN);
}

uint16_t adcMeasure(void)
{
    ADCSRA |= (1 << ADSC); // Start conversion
    
    while(ADCSRA & (1 << ADSC));
    
    return (uint16_t)(ADC & 0x3FF);
}

uint16_t adcMap(uint16_t value, uint16_t lower, uint16_t upper)
{
    float fraction = (float)value / ADC_RANGE;
    
    uint16_t range = upper - lower;
    
    return lower + (fraction * range);
}