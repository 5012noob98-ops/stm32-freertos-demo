#ifndef KEY_H
#define KEY_H

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "led.h"
#include "led_task.h"
#include "log.h"

/**
 * @brief ����״̬ö��
 */
typedef enum{
    
    KEY_STATE_WAIT,              ///< �ȴ�����״̬
    KEY_STATE_SHORT_PRESS,       ///< �̰�״̬
    KEY_STATE_LONG_PRESS,        ///< ����״̬
    KEY_STATE_DOUBLE_WAIT,       ///< �ȴ�˫��״̬
    KEY_STATE_BLINKING,          ///< ��˸״̬
    KEY_STATE_CONFIG_WAIT,       ///< �ȴ�����״̬
    KEY_STATE_CONFIG_MODE,       ///< ����ģʽ״̬
    KEY_STATE_KEY1_ADD,          ///< KEY1����ģʽ
    KEY_STATE_KEY2_MINUS,        ///< KEY2����ģʽ
    KEY_STATE_KEY2_CONFIG,       ///< KEY2����ģʽ
    KEY_STATE_SAVE_CONFIG        ///< ��������״̬

}KeyState;


/* �������� */
void key_init(void);
void key_press_handler(void);
void reset_key_state(void);
KeyState get_key_state(void);


#endif
