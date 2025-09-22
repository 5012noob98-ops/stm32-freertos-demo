#include "led.h"
#include "gpio.h"


/**
 * @brief 打开LED1
 */
void LED1_ON(void){
    HAL_GPIO_WritePin(GPIOA,LED1_Pin,GPIO_PIN_RESET);
}

/**
 * @brief 打开LED2
 */
void LED2_ON(void){
    HAL_GPIO_WritePin(GPIOA,LED2_Pin,GPIO_PIN_RESET);
    
}

/**
 * @brief 打开LED3
 */
void LED3_ON(void){
    HAL_GPIO_WritePin(GPIOA,LED3_Pin,GPIO_PIN_RESET);
    
}

/**
 * @brief 关闭LED1
 */
void LED1_OFF(void){
    HAL_GPIO_WritePin(GPIOA,LED1_Pin,GPIO_PIN_SET);
    
}

/**
 * @brief 关闭LED2
 */
void LED2_OFF(void){
    HAL_GPIO_WritePin(GPIOA,LED2_Pin,GPIO_PIN_SET);
    
}

/**
 * @brief 关闭LED3
 */
void LED3_OFF(void){
    HAL_GPIO_WritePin(GPIOA,LED3_Pin,GPIO_PIN_SET);
    
}

/**
 * @brief 打开所有LED
 */
void LED_ALL_ON(void){
    HAL_GPIO_WritePin(GPIOA,LED1_Pin|LED2_Pin|LED3_Pin,GPIO_PIN_RESET);
    
}

/**
 * @brief 关闭所有LED
 */
void LED_ALL_OFF(void){
    HAL_GPIO_WritePin(GPIOA,LED1_Pin|LED2_Pin|LED3_Pin,GPIO_PIN_SET);
}

/**
 * @brief 切换LED状态
 * @param mode 闪烁模式
 */
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
