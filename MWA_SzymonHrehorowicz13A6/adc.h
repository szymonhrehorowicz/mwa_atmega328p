/*
 * adc.h
 *
 * Created: 1/18/2024 2:11:34 PM
 *  Author: Szymon
 */ 


#ifndef ADC_H_
#define ADC_H_

/*
    INCLUDES
*/

#include <stdint.h>

/*
    FUNCTIONS
*/

void adcInit(void);
void adcOff(void);
void adcOn(void);
uint16_t adcMeasure(void);
uint16_t adcMap(uint16_t value, uint16_t lower, uint16_t upper);

#endif /* ADC_H_ */