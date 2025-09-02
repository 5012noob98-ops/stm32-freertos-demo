#ifndef LED_TASK_H
#define LED_TASK_H


#include "cmsis_os.h"
#include "task.h"
#include "stdio.h"

typedef enum{
    LED_MODE_OFF = 0,
    LED_MODE_SINGLE1,
    LED_MODE_SINGLE2,
    LED_MODE_SINGLE3,
    LED_MODE_ALL_ON,
    LED_MODE_WARNING,
    LED_MODE_CONFIG,
    LED_MODE_QUICK_BLINK,
}LED_MODE_t;

void StartLedTask(void *argument);
void Read_LedState_Task(void *argument);
void Led_task_init(void);


#endif
