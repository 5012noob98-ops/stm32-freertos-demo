#ifndef KEY_TASK_H
#define KEY_TASK_H

#include "task.h"
#include "key.h"
#include "led_task.h"

/**
 * @brief LED�¼�����ö�ٶ���
 * 
 * ����LED����������ݲ�ͬ�¼�����������LED״̬�仯
 */
typedef enum {
    LED_EVENT_MODE,   // �л�ģʽ
    LED_EVENT_FREQ,   // �޸���˸Ƶ��
    LED_EVENT_SHORT_PRESS //�û��̰�
} LED_EventType_t;

/*
 * LED�¼��ṹ�嶨��
 *
 * ����������䴫��LED�����¼���Ϣ
 */
typedef struct {
    LED_EventType_t type;   // �¼�����
    uint32_t value;         // �¼�ֵ
} LED_Event_t;


void Key_Scan_Task(void *argument);
void key_task_init(void);
void KeyEvent_SendMode(LED_MODE_t mode);
void KeyEvent_SendFreq(uint32_t freq);
void KeyEvent_SendShortPress(void);


#endif
