#ifndef LED_TASK_H
#define LED_TASK_H


#include "cmsis_os.h"
#include "task.h"
#include "stdio.h"

// LEDģʽ�������������
typedef enum{
    LED_MODE_OFF = 0,        // LED�ر�
    LED_MODE_SINGLE1,        // ����LED1
    LED_MODE_SINGLE2,        // ����LED2
    LED_MODE_SINGLE3,        // ����LED3
    LED_MODE_ALL_ON,         // ��������LED
    LED_MODE_WARNING,        // ����ģʽ��LED3��˸��
    LED_MODE_CONFIG,         // ����ģʽ��LED1��LED2��˸��
    LED_MODE_QUICK_BLINK,    // ������˸ģʽ������LED������˸��
}LED_MODE_t;

void StartLedTask(void *argument);
void Read_LedState_Task(void *argument);
void Led_task_init(void);


#endif
