#ifndef KEY_TASK_H
#define KEY_TASK_H

#include "task.h"
#include "key.h"
#include "led_task.h"

/**
 * @brief LED事件类型枚举定义
 * 
 * 用于LED控制任务根据不同事件类型来处理LED状态变化
 */
typedef enum {
    LED_EVENT_MODE,   // 切换模式
    LED_EVENT_FREQ,   // 修改闪烁频率
    LED_EVENT_SHORT_PRESS //用户短按
} LED_EventType_t;

/*
 * LED事件结构体定义
 *
 * 用于在任务间传递LED控制事件信息
 */
typedef struct {
    LED_EventType_t type;   // 事件类型
    uint32_t value;         // 事件值
} LED_Event_t;


void Key_Scan_Task(void *argument);
void key_task_init(void);
void KeyEvent_SendMode(LED_MODE_t mode);
void KeyEvent_SendFreq(uint32_t freq);
void KeyEvent_SendShortPress(void);


#endif
