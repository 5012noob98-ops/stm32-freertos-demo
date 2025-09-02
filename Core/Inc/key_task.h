#ifndef KEY_TASK_H
#define KEY_TASK_H

#include "task.h"
#include "key.h"
#include "led_task.h"

typedef enum {
    LED_EVENT_MODE,   // �л�ģʽ
    LED_EVENT_FREQ    // �޸���˸Ƶ��
} LED_EventType_t;

typedef struct {
    LED_EventType_t type;   // �¼�����
    uint32_t value;         // �¼�ֵ
} LED_Event_t;


void Key_Scan_Task(void *argument);
void key_task_init(void);
void KeyEvent_SendMode(LED_MODE_t mode);
void KeyEvent_SendFreq(uint32_t freq);


#endif
