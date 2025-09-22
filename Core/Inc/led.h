#ifndef LED_H
#define LED_H

#include "main.h"
#include "log.h"

//LEDģʽ�����ײ�����
typedef enum{
    WARNING = 0,    // ����ģʽ ��LED3��˸��
    CONFIG,         // ����ģʽ��LED1��LED2��˸��
    QUICK_BLINK,    // ������˸ģʽ ������LED������˸��
}Blink_mode;



//��������

void LED1_ON(void);
void LED2_ON(void);
void LED3_ON(void);

void LED1_OFF(void);
void LED2_OFF(void);
void LED3_OFF(void);

void LED_ALL_ON(void);
void LED_ALL_OFF(void);

void LED_TOGGLE(Blink_mode mode);


#endif
