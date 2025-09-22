#include "led_task.h"
#include "led.h"
#include "key_task.h"
#include "queue.h"


/* ˽�б��� */
static uint32_t task_led_tim = 300;              // LED����ά����LEDƵ��ֵ
static LED_MODE_t led_mode = LED_MODE_OFF;       // ��ǰLEDģʽ
static TaskHandle_t LedTaskHandle = NULL;        // LED��������
static TaskHandle_t Read_LedState_TaskHandle = NULL;  // LED״̬��ȡ������

extern QueueHandle_t xLedQueue; 

/**
 * @brief ��ʼ��LED����
 */
void Led_task_init(void){
    BaseType_t xReturned;
    
    // ����LED������
    xReturned = xTaskCreate(StartLedTask,
                            "LedTask",
                            128,
                            NULL,
                            1,
                            &LedTaskHandle);

    if (xReturned == pdPASS)
    {
        LOG_INFO("[LED] ����1�����ɹ�!\r\n");
    }

    // ����LED״̬��ȡ����
    xReturned = xTaskCreate(Read_LedState_Task,
                            "Read_ledstate_Task",
                            128,
                            NULL,
                            1,
                            &Read_LedState_TaskHandle);

    if (xReturned == pdPASS)
    {
        LOG_INFO("[LED] ����2�����ɹ�!\r\n");
    }
}

/**
 * @brief ��ȡLED״̬����
 * @param argument �������
 */
void Read_LedState_Task(void *argument){
    LED_Event_t evt;  // �¼�����

    while (1)
    {
        // �Ӷ��н���LED�¼�
        if (xQueueReceive(xLedQueue, &evt, portMAX_DELAY) == pdPASS) {
            switch (evt.type) {
                case LED_EVENT_SHORT_PRESS:
                    led_mode = (LED_MODE_t)((led_mode + 1)% 4); //��֤��LED1-2-3-0֮���л�
                    break;
                case LED_EVENT_MODE:
                    led_mode = (LED_MODE_t)evt.value;           //ֱ���л���Ӧģʽ
                    break;
                case LED_EVENT_FREQ:
                    // ����Ƶ�ʷ�Χ��50-1000ms֮��
                    if (evt.value < 50) evt.value = 50;
                    if (evt.value > 1000) evt.value = 1000;
                    task_led_tim = evt.value;
                    break;
            }
        }
    }  
}

/**
 * @brief LED��������
 * @param argument �������
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

