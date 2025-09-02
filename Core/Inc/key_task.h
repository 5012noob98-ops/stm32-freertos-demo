#ifndef KEY_TASK_H
#define KEY_TASK_H

#include "task.h"
#include "key.h"
#include "led_task.h"

typedef enum {
    LED_EVENT_MODE,   // 切换模式
    LED_EVENT_FREQ    // 修改闪烁频率
} LED_EventType_t;

typedef struct {
    LED_EventType_t type;   // 事件类型
    uint32_t value;         // 事件值
} LED_Event_t;


void Key_Scan_Task(void *argument);
void key_task_init(void);
void KeyEvent_SendMode(LED_MODE_t mode);
void KeyEvent_SendFreq(uint32_t freq);


#endif
