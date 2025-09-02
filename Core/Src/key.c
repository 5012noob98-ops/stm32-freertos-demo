#include "key.h"
#include "gpio.h"
#include "led_task.h"  // 添加LED任务头文件
#include <stdio.h>     // 添加标准输入输出头文件
#include "task.h"
#include "key_task.h"
#include "led.h"

/* 外部变量 */
extern LED_MODE_t g_led_mode;
extern uint32_t LED_TIM;

/* 私有变量 */
static uint8_t short_press_count = 0;
static uint8_t threshold_set = 0;
KeyState current_key_state = KEY_STATE_WAIT;
static uint32_t last_tick_time = 0;
static uint32_t last_click_time = 0;
static LED_MODE_t short_or_quick_mode = LED_MODE_OFF;
#define TIME_AFTER(now,old,interval) ((uint32_t)((now)-(old)) >= (interval))

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
 * @brief 按键初始化函数
 */
void key_init(void)
{
    current_key_state = KEY_STATE_WAIT;
    short_press_count = 0;
    // threshold_set = 0;
}

/**
 * @brief 按键处理主函数
 */
void key_press_handler(void)
{
    state_table[current_key_state]();
}

/**
 * @brief 重置按键状态
 */
void reset_key_state(void)
{
    current_key_state = KEY_STATE_WAIT;
}


/**
 * @brief 获取当前按键状态
 * @return 当前按键状态
 */
KeyState get_key_state(void)
{
    return current_key_state;
}

/**
 * @brief 等待按键状态处理
 */
static void state_handler_wait(void)
{

    if (HAL_GPIO_ReadPin(GPIOC, KEY1_Pin) == RESET && HAL_GPIO_ReadPin(GPIOC, KEY2_Pin) == RESET) {
        printf("按键1\r\n");
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
            if (short_press_count >= 2)
            {
                current_key_state = KEY_STATE_DOUBLE_WAIT;
                printf("进入双击模式\n");
            }else
            {
                last_click_time = xTaskGetTickCount();
                short_or_quick_mode = (LED_MODE_t)((g_led_mode + 1)% 4);
                KeyEvent_SendMode(short_or_quick_mode);
                printf("KEY1短按,切换LED灯\r\n");
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
        printf("长按结束\r\n");
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
        short_or_quick_mode = (LED_MODE_t)((g_led_mode + 1)% 4);
        KeyEvent_SendMode(short_or_quick_mode);
        printf("超时,不是双击,切换LED灯\r\n");
        if (HAL_GPIO_ReadPin(GPIOC,KEY1_Pin) == SET)
        {
            current_key_state = KEY_STATE_WAIT;
        }     
    }else{
        // 未超时，确认双击，进入闪烁状态
        short_press_count = 0;
        KeyEvent_SendMode(LED_MODE_OFF);
        printf("进入闪烁状态\r\n");
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
            current_key_state = KEY_STATE_CONFIG_MODE;
            printf("key1和key2按下3s,进入配置模式\r\n");
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
            uint32_t new_freq = get_led_tim() + 50;
            KeyEvent_SendFreq(new_freq);
            printf("LED_TIM + 50 = %lu\n", get_led_tim());
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
            printf("KEY2短按,进入减少模式\n");
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
            uint32_t new_freq = get_led_tim() - 50;
            KeyEvent_SendFreq(new_freq);
            printf("LED_TIM - 50 = %lu\n", get_led_tim());
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
        printf("保存成功，返回等待按键状态。");
        threshold_set = 0;
        current_key_state = KEY_STATE_WAIT;
    }
}

