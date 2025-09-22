#ifndef LED_H
#define LED_H

#include "main.h"
#include "log.h"

//LED模式——底层驱动
typedef enum{
    WARNING = 0,    // 警告模式 （LED3闪烁）
    CONFIG,         // 配置模式（LED1和LED2闪烁）
    QUICK_BLINK,    // 快速闪烁模式 （所有LED快速闪烁）
}Blink_mode;



//函数声明

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
