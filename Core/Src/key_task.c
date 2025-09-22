#include "key.h"
#include "key_task.h"
#include "task.h"
#include "queue.h"
#include "led_task.h"

/* ˽�б��� */
static TaskHandle_t KeyTaskHandle = NULL;

QueueHandle_t xLedQueue; // LEDģʽ��ѯ�Ķ��о��

/**
 * @brief ��ʼ����������
 * 
 * ����LED�¼����в���������ɨ������
 */
void key_task_init(void){
    // �������У����ɴ洢5��LED�¼�
    xLedQueue = xQueueCreate(5, sizeof(LED_Event_t));

    BaseType_t xReturned;
    // ��������ɨ������
    xReturned = xTaskCreate(Key_Scan_Task,
                            "Key_task1",
                            128,
                            NULL,
                            1,
                            &KeyTaskHandle);
    if (xReturned == pdPASS)
    {
        LOG_INFO("[KEY] ����3�����ɹ�!\r\n");      
    }
}

/**
 * @brief ����ģʽ�л��¼�
 * 
 * ��ģʽ�л��¼�ͨ�����з��Ͳ�֪ͨLED����״̬�л�
 * @param mode LEDģʽ
 */
void KeyEvent_SendMode(LED_MODE_t mode) {
    LED_Event_t evt = { .type = LED_EVENT_MODE, .value = mode };
    xQueueSend(xLedQueue, &evt, 0);
}

/**
 * @brief ����LED��˸Ƶ���޸��¼�
 * 
 * ��Ƶ���޸��¼�ͨ�����з��Ͳ�֪ͨLED�����޸���˸Ƶ��
 * @param freq ��˸Ƶ��
 */
void KeyEvent_SendFreq(uint32_t freq) {
    LED_Event_t evt = { .type = LED_EVENT_FREQ, .value = freq };
    xQueueSend(xLedQueue, &evt, 0);
}

/**
 * @brief ����LEDģʽѡ���¼�
 * 
 * ����Ӧ��ѡ��״̬�¼�ͨ�����з��Ͳ�֪ͨLED����״̬�л�
 */
void KeyEvent_SendShortPress(void) {
    LED_Event_t evt = { .type = LED_EVENT_SHORT_PRESS, .value = 0 };
    xQueueSend(xLedQueue, &evt, 0);
}

/**
 * @brief ����ɨ������
 * 
 * ����ɨ�谴��״̬���������¼�
 * @param argument �������
 */
void Key_Scan_Task(void *argument){

    while (1)
    {
        key_press_handler();    
    }
    
}
