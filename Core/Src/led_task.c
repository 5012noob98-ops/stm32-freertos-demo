#include "led_task.h"
#include "led.h"
#include "key_task.h"
#include "queue.h"


/* 私有变量 */
static uint32_t task_led_tim = 300;              // LED任务维护的LED频率值
static LED_MODE_t led_mode = LED_MODE_OFF;       // 当前LED模式
static TaskHandle_t LedTaskHandle = NULL;        // LED主任务句柄
static TaskHandle_t Read_LedState_TaskHandle = NULL;  // LED状态读取任务句柄

extern QueueHandle_t xLedQueue; 

/**
 * @brief 初始化LED任务
 */
void Led_task_init(void){
    BaseType_t xReturned;
    
    // 创建LED主任务
    xReturned = xTaskCreate(StartLedTask,
                            "LedTask",
                            128,
                            NULL,
                            1,
                            &LedTaskHandle);

    if (xReturned == pdPASS)
    {
        LOG_INFO("[LED] 任务1创建成功!\r\n");
    }

    // 创建LED状态读取任务
    xReturned = xTaskCreate(Read_LedState_Task,
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

/**
 * @brief 读取LED状态任务
 * @param argument 任务参数
 */
void Read_LedState_Task(void *argument){
    LED_Event_t evt;  // 事件参数

    while (1)
    {
        // 从队列接收LED事件
        if (xQueueReceive(xLedQueue, &evt, portMAX_DELAY) == pdPASS) {
            switch (evt.type) {
                case LED_EVENT_SHORT_PRESS:
                    led_mode = (LED_MODE_t)((led_mode + 1)% 4); //保证在LED1-2-3-0之间切换
                    break;
                case LED_EVENT_MODE:
                    led_mode = (LED_MODE_t)evt.value;           //直接切换对应模式
                    break;
                case LED_EVENT_FREQ:
                    // 限制频率范围在50-1000ms之间
                    if (evt.value < 50) evt.value = 50;
                    if (evt.value > 1000) evt.value = 1000;
                    task_led_tim = evt.value;
                    break;
            }
        }
    }  
}

/**
 * @brief LED主任务函数
 * @param argument 任务参数
 */
void StartLedTask(void *argument){
   
    while(1){
        switch (led_mode)
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
                vTaskDelay(task_led_tim);
                break;  

            case LED_MODE_QUICK_BLINK:
                LED_TOGGLE(QUICK_BLINK);
                vTaskDelay(200);
                break;  
        }
    }
}

