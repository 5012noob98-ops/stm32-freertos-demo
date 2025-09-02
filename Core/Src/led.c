#include "led.h"
#include "gpio.h"

/* 私有变量 */
uint32_t led_tim = 300;      //LED闪烁时间间隔

void LED1_ON(void){
    HAL_GPIO_WritePin(GPIOA,LED1_Pin,GPIO_PIN_RESET);
}

void LED2_ON(void){
    HAL_GPIO_WritePin(GPIOA,LED2_Pin,GPIO_PIN_RESET);
    
}

void LED3_ON(void){
    HAL_GPIO_WritePin(GPIOA,LED3_Pin,GPIO_PIN_RESET);
    
}

void LED1_OFF(void){
    HAL_GPIO_WritePin(GPIOA,LED1_Pin,GPIO_PIN_SET);
    
}

void LED2_OFF(void){
    HAL_GPIO_WritePin(GPIOA,LED2_Pin,GPIO_PIN_SET);
    
}

void LED3_OFF(void){
    HAL_GPIO_WritePin(GPIOA,LED3_Pin,GPIO_PIN_SET);
    
}

void LED_ALL_ON(void){
    HAL_GPIO_WritePin(GPIOA,LED1_Pin|LED2_Pin|LED3_Pin,GPIO_PIN_RESET);
    
}

void LED_ALL_OFF(void){
    HAL_GPIO_WritePin(GPIOA,LED1_Pin|LED2_Pin|LED3_Pin,GPIO_PIN_SET);
}

void LED_TOGGLE(Blink_mode mode){
    
    switch (mode)
    {
    case WARNING:
        HAL_GPIO_TogglePin(GPIOA,LED3_Pin);
    break;
    case CONFIG:
        HAL_GPIO_TogglePin(GPIOA,LED1_Pin|LED2_Pin);
    break;
    case QUICK_BLINK:
        HAL_GPIO_TogglePin(GPIOA,LED1_Pin|LED2_Pin|LED3_Pin);
    break;
    } 
}


/**
 * @brief 获取LED闪烁时间间隔
 * @return 当前LED闪烁时间间隔
 */
uint32_t get_led_tim(void)
{
    return led_tim;
}
