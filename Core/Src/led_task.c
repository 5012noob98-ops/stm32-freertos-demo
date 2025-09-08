#include "led_task.h"
#include "led.h"
#include "key_task.h"
#include "queue.h"

extern uint32_t led_tim;
extern QueueHandle_t xLedQueue;
LED_MODE_t g_led_mode = LED_MODE_OFF;
static TaskHandle_t LedTaskHandle = NULL;
static TaskHandle_t Read_LedState_TaskHandle = NULL;

void Led_task_init(void){
    
    BaseType_t xReturned;
    xReturned = xTaskCreate(  StartLedTask,
                "LedTask",
                128,
                NULL,
                1,
                &LedTaskHandle);
    if (xReturned == pdPASS)
    {
        LOG_INFO("[LED] 任务1创建成功!\r\n");
    }

        xReturned = xTaskCreate(  Read_LedState_Task,
                "Read_ledstate_Task",
                128,
                NULL,
                1,
                &Read_LedState_TaskHandle);
    if (xReturned == pdPASS)
    {
        LOG_INFO("[LED] 任务2创建成功!\r\n");
    }
    
}

void Read_LedState_Task(void *argument){
    LED_Event_t evt;  // 添加变量声明
    while (1)
    {
        
        if (xQueueReceive(xLedQueue, &evt, portMAX_DELAY) == pdPASS) {
            switch (evt.type) {
                case LED_EVENT_MODE:
                    g_led_mode = (LED_MODE_t)evt.value;
                    break;
                case LED_EVENT_FREQ:
                    if (evt.value < 50) evt.value = 50;
                    if (evt.value > 1000) evt.value = 1000;
                    led_tim = evt.value;
                    break;
            }
        }
    }
    
}

void StartLedTask(void *argument){
   
    while(1){
        switch (g_led_mode)
        {
            case LED_MODE_OFF:
                LED_ALL_OFF();
                break;
            case LED_MODE_SINGLE1:
                LED1_ON();
                LED2_OFF();
                LED3_OFF();
                break;
            case LED_MODE_SINGLE2:
                LED2_ON();
                LED1_OFF();
                LED3_OFF();         
                break;
            case LED_MODE_SINGLE3:
                LED3_ON();
                LED2_OFF();
                LED1_OFF();
                break;
            case LED_MODE_ALL_ON:
                LED_ALL_ON();
                break;
            case LED_MODE_WARNING:
                LED_TOGGLE(WARNING);
                vTaskDelay(500);
                break;
            case LED_MODE_CONFIG:
                LED_TOGGLE(CONFIG);
                vTaskDelay(led_tim);
                break;  
            case LED_MODE_QUICK_BLINK:
                LED_TOGGLE(QUICK_BLINK);
                vTaskDelay(200);
                break;  
        }
    }
}

