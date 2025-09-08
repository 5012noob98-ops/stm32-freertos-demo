#include "key.h"
#include "key_task.h"
#include "task.h"
#include "queue.h"
#include "led_task.h"

extern LED_MODE_t g_led_mode;
extern KeyState current_key_state;
static TaskHandle_t KeyTaskHandle = NULL;
QueueHandle_t xLedQueue;


void key_task_init(void){
    
    xLedQueue = xQueueCreate(5, sizeof(LED_Event_t));

    BaseType_t xReturned;
    xReturned = xTaskCreate(  Key_Scan_Task,
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

void KeyEvent_SendMode(LED_MODE_t mode) {
    LED_Event_t evt = { .type = LED_EVENT_MODE, .value = mode };
    xQueueSend(xLedQueue, &evt, 0);
}

void KeyEvent_SendFreq(uint32_t freq) {
    LED_Event_t evt = { .type = LED_EVENT_FREQ, .value = freq };
    xQueueSend(xLedQueue, &evt, 0);
}

void Key_Scan_Task(void *argument){

    while (1)
    {
        key_press_handler();    
    }
    
}
