#include "key.h"
#include "key_task.h"
#include "task.h"
#include "queue.h"
#include "led_task.h"

/* 私有变量 */
static TaskHandle_t KeyTaskHandle = NULL;

QueueHandle_t xLedQueue; // LED模式查询的队列句柄

/**
 * @brief 初始化按键任务
 * 
 * 创建LED事件队列并创建按键扫描任务
 */
void key_task_init(void){
    // 创建队列，最多可存储5个LED事件
    xLedQueue = xQueueCreate(5, sizeof(LED_Event_t));

    BaseType_t xReturned;
    // 创建按键扫描任务
    xReturned = xTaskCreate(Key_Scan_Task,
                            "Key_task1",
                            128,
                            NULL,
                            1,
                            &KeyTaskHandle);
    if (xReturned == pdPASS)
    {
        LOG_INFO("[KEY] 任务3创建成功!\r\n");      
    }
}

/**
 * @brief 发送模式切换事件
 * 
 * 将模式切换事件通过队列发送并通知LED任务状态切换
 * @param mode LED模式
 */
void KeyEvent_SendMode(LED_MODE_t mode) {
    LED_Event_t evt = { .type = LED_EVENT_MODE, .value = mode };
    xQueueSend(xLedQueue, &evt, 0);
}

/**
 * @brief 发送LED闪烁频率修改事件
 * 
 * 将频率修改事件通过队列发送并通知LED任务修改闪烁频率
 * @param freq 闪烁频率
 */
void KeyEvent_SendFreq(uint32_t freq) {
    LED_Event_t evt = { .type = LED_EVENT_FREQ, .value = freq };
    xQueueSend(xLedQueue, &evt, 0);
}

/**
 * @brief 发送LED模式选择事件
 * 
 * 将对应的选择状态事件通过队列发送并通知LED任务状态切换
 */
void KeyEvent_SendShortPress(void) {
    LED_Event_t evt = { .type = LED_EVENT_SHORT_PRESS, .value = 0 };
    xQueueSend(xLedQueue, &evt, 0);
}

/**
 * @brief 按键扫描任务
 * 
 * 持续扫描按键状态并处理按键事件
 * @param argument 任务参数
 */
void Key_Scan_Task(void *argument){

    while (1)
    {
        key_press_handler();    
    }
    
}
