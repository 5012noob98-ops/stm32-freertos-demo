#include "key.h"
#include "gpio.h"
#include "led_task.h"  
#include <stdio.h>     
#include "task.h"
#include "key_task.h"
#include "led.h"



/* 私有变量 */

static uint8_t short_press_count = 0;                   //短按计数值
static uint8_t threshold_set = 0;                       //配置模式的标志位
static KeyState current_key_state = KEY_STATE_WAIT;     //当前按键状态
static uint32_t last_tick_time = 0;                     //按键切换时间戳
static uint32_t last_click_time = 0;                    //短按记录时间戳，用于记录上一次短按间隔，判断是否是短按还是快速双击
static uint32_t key_led_tim = 300;                      //LED配置模式下的闪烁频率，用写队列的方式对齐led_task的闪烁频率，初始值对齐300

/* 宏定义 */

#define TIME_AFTER(now,old,interval) ((uint32_t)((now)-(old)) >= (interval)) //计算新旧时间差，用于按键切换的判断

/* 私有函数声明 */

static void state_handler_wait(void);
static void state_handler_short_press(void);
static void state_handler_long_press(void);
static void state_handler_double_wait(void);
static void state_handler_blinking(void);
static void state_handler_config_wait(void);
static void state_handler_config_mode(void);
static void state_handler_key1_add(void);
static void state_handler_key2_minus(void);
static void state_handler_key2_config(void);
static void state_handler_save_config(void);

/* 状态处理函数表 */
static void (*state_table[])(void) = {
    [KEY_STATE_WAIT]        = state_handler_wait,
    [KEY_STATE_SHORT_PRESS] = state_handler_short_press,
    [KEY_STATE_LONG_PRESS]  = state_handler_long_press,
    [KEY_STATE_DOUBLE_WAIT] = state_handler_double_wait,
    [KEY_STATE_BLINKING]    = state_handler_blinking,
    [KEY_STATE_CONFIG_WAIT] = state_handler_config_wait,
    [KEY_STATE_CONFIG_MODE] = state_handler_config_mode,
    [KEY_STATE_KEY1_ADD]    = state_handler_key1_add,
    [KEY_STATE_KEY2_MINUS]  = state_handler_key2_minus,
    [KEY_STATE_KEY2_CONFIG] = state_handler_key2_config,
    [KEY_STATE_SAVE_CONFIG] = state_handler_save_config,
};

/**
 * @brief 按键处理函数
 */
void key_press_handler(void)
{
    state_table[current_key_state]();
}



/**
 * @brief 等待按键状态处理
 */
static void state_handler_wait(void)
{
    if (HAL_GPIO_ReadPin(GPIOC, KEY1_Pin) == RESET && HAL_GPIO_ReadPin(GPIOC, KEY2_Pin) == RESET) {
        LOG_INFO("按键1和2同时按下\r\n");
        current_key_state = KEY_STATE_CONFIG_WAIT;
        last_tick_time = xTaskGetTickCount();
    }
    else if (HAL_GPIO_ReadPin(GPIOC, KEY1_Pin) == RESET) {
        current_key_state = KEY_STATE_SHORT_PRESS;
        last_tick_time = xTaskGetTickCount();
    }
}

/**
 * @brief 短按状态处理
 */
static void state_handler_short_press(void)
{
    if (HAL_GPIO_ReadPin(GPIOC,KEY1_Pin) == SET)
    {   
        if (TIME_AFTER(xTaskGetTickCount(),last_tick_time,20) && !TIME_AFTER(xTaskGetTickCount(),last_tick_time,2000))
        {
            short_press_count++;
            if (short_press_count >= 2) //短按次数大于2，进入双击模式
            {
                current_key_state = KEY_STATE_DOUBLE_WAIT;
                LOG_INFO("进入双击模式\n");
            }else
            {
                last_click_time = xTaskGetTickCount();
                KeyEvent_SendShortPress();   //短按确认，发送短按通知
                LOG_INFO("KEY1短按,切换LED灯\r\n");
                if (HAL_GPIO_ReadPin(GPIOC,KEY1_Pin) == SET)
                {
                    current_key_state = KEY_STATE_WAIT;
                }     
            }
        }
    }else if (TIME_AFTER(xTaskGetTickCount(),last_tick_time,2000))
    {
        current_key_state = KEY_STATE_LONG_PRESS;
        last_tick_time = xTaskGetTickCount();
    }
}

/**
 * @brief 长按状态处理
 */
static void state_handler_long_press(void)
{
    KeyEvent_SendMode(LED_MODE_OFF);
    if (HAL_GPIO_ReadPin(GPIOC, KEY1_Pin) == SET) {
        LOG_INFO("长按结束\r\n");
        current_key_state = KEY_STATE_WAIT;
    }
}

/**
 * @brief 等待双击状态处理
 */
static void state_handler_double_wait(void)
{
    // 从第一次释放开始检查是否超时300ms，超时则认为是单击
    if (TIME_AFTER(xTaskGetTickCount(), last_click_time, 500)) {
        // 超时，返回单击模式
        last_click_time = xTaskGetTickCount();
        short_press_count++;
        KeyEvent_SendShortPress();  //短按确认，发送短按通知
        LOG_INFO("超时,不是双击,切换LED灯\r\n");
        if (HAL_GPIO_ReadPin(GPIOC,KEY1_Pin) == SET)
        {
            current_key_state = KEY_STATE_WAIT;
        }     
    }else{
        // 未超时，确认双击，进入闪烁状态
        short_press_count = 0;
        KeyEvent_SendMode(LED_MODE_OFF);
        vTaskDelay(1);
        LOG_INFO("进入闪烁状态\r\n");
        current_key_state = KEY_STATE_BLINKING;
    }
}


/**
 * @brief 闪烁状态处理
 */
static void state_handler_blinking(void)
{
    KeyEvent_SendMode(LED_MODE_QUICK_BLINK);
    current_key_state = KEY_STATE_WAIT;
}


/**
 * @brief 等待配置状态处理
 */
static void state_handler_config_wait(void)
{
    // 检查双键是否仍然按下
    if ((HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == RESET) && 
        (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == RESET)) {
        
        if (TIME_AFTER(xTaskGetTickCount(), last_tick_time, 3000)) {
            // 按下3秒，进入配置模式
            KeyEvent_SendMode(LED_MODE_OFF);
            vTaskDelay(1);
            current_key_state = KEY_STATE_CONFIG_MODE;
            LOG_INFO("key1和key2按下3s,进入配置模式\r\n");
        }
    } else {
        // 双键释放，未达到配置时间，返回初始状态
        current_key_state = KEY_STATE_WAIT;
    }
}

/**
 * @brief 配置模式状态处理
 */
static void state_handler_config_mode(void)
{
    KeyEvent_SendMode(LED_MODE_CONFIG);
    // 用于判断是否可以进入add/minus操作的时间
    static uint32_t enter_threshold_time = 0;

    // 初始化进入配置状态的时间，只在进入状态时执行一次
    if (!threshold_set) {
        enter_threshold_time = xTaskGetTickCount();
        threshold_set = 1;
    }

    if (TIME_AFTER(xTaskGetTickCount(), enter_threshold_time, 1000)) {
    // 按键选择
    if (HAL_GPIO_ReadPin(GPIOC, KEY1_Pin) == RESET) {
        last_tick_time = xTaskGetTickCount();
        current_key_state = KEY_STATE_KEY1_ADD; // 进入增加模式
    }
    else if (HAL_GPIO_ReadPin(GPIOC, KEY2_Pin) == RESET) {
        last_tick_time = xTaskGetTickCount();
        current_key_state = KEY_STATE_KEY2_CONFIG; // 进入减少/配置模式
    }
    }
}


/**
 * @brief KEY1增加模式处理，增加LED的闪烁间隔，每一次增加50ms
 */
static void state_handler_key1_add(void)
{
    // 检查按键是否释放（短按确认）
    if (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == SET) {
        if (TIME_AFTER(xTaskGetTickCount(), last_tick_time, 20)) {
            key_led_tim = key_led_tim + 50;
            if (key_led_tim >= 1000)
            {
                key_led_tim = 1000;
            }            
            KeyEvent_SendFreq(key_led_tim);
            LOG_INFO("key_led_tim + 50 = %lu\n", key_led_tim);
        }
        current_key_state = KEY_STATE_CONFIG_MODE;
    }
}


/**
 * @brief KEY2配置模式处理，短按进入KEY2减少模式处理，长按3s进入保存配置模式处理
 */
static void state_handler_key2_config(void)
{
    // 检查按键是否释放
    if (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == SET) {
        // 短按进入减少模式
        if (TIME_AFTER(xTaskGetTickCount(), last_tick_time, 20) && !TIME_AFTER(xTaskGetTickCount(), last_tick_time, 3000)) {
            current_key_state = KEY_STATE_KEY2_MINUS;
            LOG_INFO("KEY2短按,进入减少模式\n");
        }
        // 长按或超时进入保存配置模式
        else {
            current_key_state = KEY_STATE_SAVE_CONFIG;
        }
    }
    // 检查是否长按
    else {
        if (TIME_AFTER(xTaskGetTickCount(), last_tick_time, 3000)) {
            current_key_state = KEY_STATE_SAVE_CONFIG;
        }
    }
}

/**
 * @brief KEY2减少模式处理，减少LED的闪烁间隔，每一次减少50ms
 */
static void state_handler_key2_minus(void)
{
    // 确认释放时间在20 < tick < 3000 范围内，执行减少模式
    if (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == SET) {
        if (TIME_AFTER(xTaskGetTickCount(), last_tick_time, 20) && 
            !TIME_AFTER(xTaskGetTickCount(), last_tick_time, 3000)) {
            key_led_tim = key_led_tim - 50;
            if (key_led_tim <= 50)
            {
                key_led_tim = 50;
            }
            KeyEvent_SendFreq(key_led_tim);
            LOG_INFO("key_led_tim - 50 = %lu\n", key_led_tim);
        }
        current_key_state = KEY_STATE_CONFIG_MODE;
    }
}

/**
 * @brief 保存配置模式处理
 */
static void state_handler_save_config(void)
{
    KeyEvent_SendMode(LED_MODE_OFF);
    if (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == SET) {
        LOG_INFO("保存成功，返回等待按键状态。");
        threshold_set = 0;
        current_key_state = KEY_STATE_WAIT;
    }
}

