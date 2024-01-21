/*
 * dataQueue.h
 *
 * Created: 1/20/2024 8:55:12 PM
 *  Author: Szymon
 */ 

#ifndef DATAQUEUE_H_
#define DATAQUEUE_H_

/*
    INCLUDES
*/

#include <avr/io.h>
#include <stdbool.h>

/*
    DEFINES
*/

#define MAX_INT_LENGTH 4u
#define MAX_CMD_LENGTH 4u

/*
    TYPE DEFINITIONS
*/

typedef struct DataQueue
{
    void   (*action)(void);
    struct DataQueue *next;
} dataQueue_t;

/*
    FUNCTIONS
*/

void dataQueueInit(void);
bool intToString(int num, uint8_t strOut[MAX_INT_LENGTH], uint8_t *strLength);
bool stringToInt(uint8_t str[MAX_INT_LENGTH], uint8_t strLength, int *num);
void printWelcome(void);
void printMode(void);
void printSpeed(void);
void printDirection(void);
void translateCmd(void);
void commandIntHandler(void);

/*
    EXTERNS
*/

extern volatile uint8_t     statusRequest;
extern          dataQueue_t dataQueueStart;
extern          uint8_t     cmdError;
extern volatile uint8_t     cmdBuff[MAX_CMD_LENGTH];     
extern volatile uint8_t     cmdLength;    
extern volatile uint8_t     cmdToExec;

#endif /* DATAQUEUE_H_ */