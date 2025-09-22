#ifndef LED_TASK_H
#define LED_TASK_H


#include "cmsis_os.h"
#include "task.h"
#include "stdio.h"

// LED模式——任务层驱动
typedef enum{
    LED_MODE_OFF = 0,        // LED关闭
    LED_MODE_SINGLE1,        // 点亮LED1
    LED_MODE_SINGLE2,        // 点亮LED2
    LED_MODE_SINGLE3,        // 点亮LED3
    LED_MODE_ALL_ON,         // 点亮所有LED
    LED_MODE_WARNING,        // 警告模式（LED3闪烁）
    LED_MODE_CONFIG,         // 配置模式（LED1和LED2闪烁）
    LED_MODE_QUICK_BLINK,    // 快速闪烁模式（所有LED快速闪烁）
}LED_MODE_t;

void StartLedTask(void *argument);
void Read_LedState_Task(void *argument);
void Led_task_init(void);


#endif
