#ifndef LED_H
#define LED_H

#include "main.h"
#include "log.h"

//��������
typedef enum{
    WARNING = 0,
    CONFIG,
    QUICK_BLINK,
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

uint32_t get_led_tim(void);

#endif
