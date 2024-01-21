/*
 * dataQueue.c
 *
 * Created: 1/20/2024 8:55:00 PM
 *  Author: Szymon
 */ 

/*
    INCLUDES
*/

#include <util/atomic.h>
#include "uart.h"
#include "uartControl.h"
#include "fan.h"

/*
    DEFINES
*/

#define NO_CMDS        3u
#define CMD            0u
#define CMD_FIRST_BIT  1u
#define CR             13u

/*
    GLOBAL VARIABLES
*/

fanHandler_t fan;

dataQueue_t dataQueueMode;
dataQueue_t dataQueueSpeed;
dataQueue_t dataQueueDirection;

uint8_t cmds[NO_CMDS] = {'v', 'd', 'm'};

dataQueue_t dataQueueStart =
{
    .action =  printWelcome,
    .next   = &dataQueueMode,
};

/*
    FUNCTIONS
*/

void dataQueueInit(void)
{
    dataQueueMode.action =  printMode;
    dataQueueMode.next   = &dataQueueSpeed;

    dataQueueSpeed.action =  printSpeed;
    dataQueueSpeed.next   = &dataQueueDirection;

    dataQueueDirection.action =  printDirection;
    dataQueueDirection.next   = &dataQueueStart;
}

bool intToString(int num, uint8_t strOut[MAX_INT_LENGTH], uint8_t *strLength)
{
    uint8_t size = MAX_INT_LENGTH;
    uint8_t idx  = 0;
    uint8_t str[MAX_INT_LENGTH];
    
    if(num > 9999)
    {
        return false;
    }
    
    str[3] =  num % 10;
    str[2] = (num / 10) % 10;
    str[1] = (num / 100) % 10;
    str[0] = (num / 1000) % 10;
    
    if(str[0] == 0)
    {
        idx++;
    }

    if((str[0] == 0) && (str[1] == 0))
    {
        idx++;
    }

    if((str[0] == 0) && (str[1] == 0) && (str[2] == 0))
    {
        idx++;
    }

    for(int i = idx; i < size; i++)
    {
        strOut[i - idx] = '0' + str[i];
    }
    
    *strLength = size - idx;
    
    return true;
}

bool stringToInt(uint8_t str[MAX_INT_LENGTH], uint8_t strLength, int *num)
{
    if(strLength > MAX_INT_LENGTH)
    {
        return false;
    }
    
    uint8_t numerical[MAX_INT_LENGTH];
    
    numerical[0] = (str[0] == '0') ? 0 : (str[0] - '0');
    numerical[1] = (str[1] == '0') ? 0 : (str[1] - '0');
    numerical[2] = (str[2] == '0') ? 0 : (str[2] - '0');
    
    switch(strLength)
    {
        case 1:
            *num = numerical[0];
            break;
        case 2:
            *num = (numerical[0] * 10) + numerical[1];
            break;
        case 3:
            *num = (numerical[0] * 100) + (numerical[1] * 10) + numerical[2];
            break;
        default:
            return false;
    }
    
    return true;
}

void printWelcome(void)
{
    static const __flash uint8_t str[] = {"Fan controller\r\n"};
    
    for(uint8_t i = 0; i < sizeof(str)/sizeof(str[0]); i++)
    {
        uartSendByte(str[i]);
    }
}

void printMode(void)
{
    uint8_t mode = fanGetMode(&fan);
    static const __flash uint8_t onStr[] =  {"Mode: ON\r\n"};
    static const __flash uint8_t offStr[] = {"Mode: OFF\r\n"};
    
    if(mode == FAN_ON)
    {
        uartSendString(onStr, sizeof(onStr)/sizeof(onStr[0]));
    }else
    {
        uartSendString(offStr, sizeof(offStr)/sizeof(offStr[0]));
    }
}

void printSpeed(void)
{
    static const __flash uint8_t speedStr[] = {"Speed: "};
    static const __flash uint8_t endStr[]   = {" %\r\n"};
    uint8_t speed = fanGetSpeed(&fan);
    uint8_t str[MAX_INT_LENGTH];
    uint8_t len;
    
    intToString((int)speed, str, &len);
    
    uartSendString(speedStr, sizeof(speedStr)/sizeof(speedStr[0]));
    uartSendString(str, len);
    uartSendString(endStr, sizeof(endStr)/sizeof(endStr[0]));
}
void printDirection(void)
{
    static const __flash uint8_t cwStr[]  = {"Direction: CW\r\n"};
    static const __flash uint8_t ccwStr[] = {"Direction: CCW\r\n"};
    static const __flash uint8_t endStr[] = {"\r\n"};
    
    uint8_t direction = fanGetDirection(&fan);
    
    if(direction == FAN_DIR_CW)
    {
        uartSendString(cwStr, sizeof(cwStr)/sizeof(cwStr[0]));
    }else
    {
        uartSendString(ccwStr, sizeof(ccwStr)/sizeof(ccwStr[0]));
    }
    
    uartSendString(endStr, sizeof(endStr)/sizeof(endStr[0]));
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if(statusRequest)
        {
            statusRequest = 0;
        }
    }
}

void translateCmd(void)
{
    uint8_t tempCmdLength = 0;
    uint8_t tempCmdBuff[MAX_CMD_LENGTH];
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        tempCmdLength = cmdLength;
        for(uint8_t i = 0; i < cmdLength; i++)
        {
            tempCmdBuff[i] = cmdBuff[i];
        }
        cmdToExec = 0;
    }
    
    switch(tempCmdBuff[CMD])
    {
        case 'v' :
        cmdError = 0;
        int newSpeed = 0;
        uint8_t noNumError = stringToInt(tempCmdBuff + 1, tempCmdLength - 1, &newSpeed);
        
        if(!noNumError)
        {
            cmdError = 1;
            break;
        }
        
        fanStatus = fanSetSpeed(&fan, (uint8_t)newSpeed);
        
        break;
        case 'd' :
        cmdError = 0;
        
        switch(tempCmdBuff[CMD_FIRST_BIT])
        {
            case '0' :
            fanStatus = fanSetDirection(&fan, FAN_DIR_CW);
            break;
            case '1' :
            fanStatus = fanSetDirection(&fan, FAN_DIR_CCW);
            break;
            default:
            cmdError = 1;
            break;
        }
        break;
        case 'm' :
        cmdError = 0;
        
        switch(tempCmdBuff[CMD_FIRST_BIT])
        {
            case '0' :
            fanStatus = fanStop(&fan);
            break;
            case '1' :
            fanStatus = fanStart(&fan);
            break;
            default:
            cmdError = 1;
            break;
        }

        break;
        default:
        cmdError = 1;
        break;
    }
}

void commandIntHandler(void)
{
    static uint8_t idx  = 0;
    uint8_t byte = UDR0;

    if(byte == 's')
    {
        
        if(!statusRequest)
        {
            statusRequest = 1;
        }
    }else if(idx)
    {
        if(byte == CR)
        {
            cmdLength = idx;
            idx = 0;
            cmdToExec = 1;
        }else
        {
            cmdBuff[idx++] = byte;
        }
    }else
    {
        for(uint8_t i = 0; i < NO_CMDS; i++)
        {
            if(byte == cmds[i])
            {
                cmdBuff[0] = byte;
                idx++;
                break;
            }
        }
    }
}