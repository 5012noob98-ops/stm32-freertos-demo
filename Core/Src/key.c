#include "key.h"
#include "gpio.h"
#include "led_task.h"  // ���LED����ͷ�ļ�
#include <stdio.h>     // ��ӱ�׼�������ͷ�ļ�
#include "task.h"
#include "key_task.h"
#include "led.h"

/* �ⲿ���� */
extern LED_MODE_t g_led_mode;
extern uint32_t LED_TIM;

/* ˽�б��� */
static uint8_t short_press_count = 0;
static uint8_t threshold_set = 0;
KeyState current_key_state = KEY_STATE_WAIT;
static uint32_t last_tick_time = 0;
static uint32_t last_click_time = 0;
static LED_MODE_t short_or_quick_mode = LED_MODE_OFF;
#define TIME_AFTER(now,old,interval) ((uint32_t)((now)-(old)) >= (interval))

/* ˽�к������� */
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

/* ״̬�������� */
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
 * @brief ������ʼ������
 */
void key_init(void)
{
    current_key_state = KEY_STATE_WAIT;
    short_press_count = 0;
    // threshold_set = 0;
}

/**
 * @brief ��������������
 */
void key_press_handler(void)
{
    state_table[current_key_state]();
}

/**
 * @brief ���ð���״̬
 */
void reset_key_state(void)
{
    current_key_state = KEY_STATE_WAIT;
}


/**
 * @brief ��ȡ��ǰ����״̬
 * @return ��ǰ����״̬
 */
KeyState get_key_state(void)
{
    return current_key_state;
}

/**
 * @brief �ȴ�����״̬����
 */
static void state_handler_wait(void)
{

    if (HAL_GPIO_ReadPin(GPIOC, KEY1_Pin) == RESET && HAL_GPIO_ReadPin(GPIOC, KEY2_Pin) == RESET) {
        printf("����1\r\n");
        current_key_state = KEY_STATE_CONFIG_WAIT;
        last_tick_time = xTaskGetTickCount();
    }
    else if (HAL_GPIO_ReadPin(GPIOC, KEY1_Pin) == RESET) {

        current_key_state = KEY_STATE_SHORT_PRESS;
        last_tick_time = xTaskGetTickCount();
    }
}

/**
 * @brief �̰�״̬����
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
                printf("����˫��ģʽ\n");
            }else
            {
                last_click_time = xTaskGetTickCount();
                short_or_quick_mode = (LED_MODE_t)((g_led_mode + 1)% 4);
                KeyEvent_SendMode(short_or_quick_mode);
                printf("KEY1�̰�,�л�LED��\r\n");
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
 * @brief ����״̬����
 */
static void state_handler_long_press(void)
{
    KeyEvent_SendMode(LED_MODE_OFF);
    if (HAL_GPIO_ReadPin(GPIOC, KEY1_Pin) == SET) {
        printf("��������\r\n");
        current_key_state = KEY_STATE_WAIT;
    }
}

/**
 * @brief �ȴ�˫��״̬����
 */
static void state_handler_double_wait(void)
{
    // �ӵ�һ���ͷſ�ʼ����Ƿ�ʱ300ms����ʱ����Ϊ�ǵ���
    if (TIME_AFTER(xTaskGetTickCount(), last_click_time, 500)) {
        // ��ʱ�����ص���ģʽ
        last_click_time = xTaskGetTickCount();
        short_press_count++;
        short_or_quick_mode = (LED_MODE_t)((g_led_mode + 1)% 4);
        KeyEvent_SendMode(short_or_quick_mode);
        printf("��ʱ,����˫��,�л�LED��\r\n");
        if (HAL_GPIO_ReadPin(GPIOC,KEY1_Pin) == SET)
        {
            current_key_state = KEY_STATE_WAIT;
        }     
    }else{
        // δ��ʱ��ȷ��˫����������˸״̬
        short_press_count = 0;
        KeyEvent_SendMode(LED_MODE_OFF);
        printf("������˸״̬\r\n");
        current_key_state = KEY_STATE_BLINKING;
    }
}


/**
 * @brief ��˸״̬����
 */
static void state_handler_blinking(void)
{
    KeyEvent_SendMode(LED_MODE_QUICK_BLINK);
    current_key_state = KEY_STATE_WAIT;
}


/**
 * @brief �ȴ�����״̬����
 */
static void state_handler_config_wait(void)
{
    // ���˫���Ƿ���Ȼ����
    if ((HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == RESET) && 
        (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == RESET)) {
        
        if (TIME_AFTER(xTaskGetTickCount(), last_tick_time, 3000)) {
            // ����3�룬��������ģʽ
            KeyEvent_SendMode(LED_MODE_OFF);
            current_key_state = KEY_STATE_CONFIG_MODE;
            printf("key1��key2����3s,��������ģʽ\r\n");
        }
    } else {
        // ˫���ͷţ�δ�ﵽ����ʱ�䣬���س�ʼ״̬
        current_key_state = KEY_STATE_WAIT;
    }
}

/**
 * @brief ����ģʽ״̬����
 */
static void state_handler_config_mode(void)
{
    KeyEvent_SendMode(LED_MODE_CONFIG);
    // �����ж��Ƿ���Խ���add/minus������ʱ��
    static uint32_t enter_threshold_time = 0;

    // ��ʼ����������״̬��ʱ�䣬ֻ�ڽ���״̬ʱִ��һ��
    if (!threshold_set) {
        enter_threshold_time = xTaskGetTickCount();
        threshold_set = 1;
    }

    if (TIME_AFTER(xTaskGetTickCount(), enter_threshold_time, 1000)) {
    // ����ѡ��
    if (HAL_GPIO_ReadPin(GPIOC, KEY1_Pin) == RESET) {
        last_tick_time = xTaskGetTickCount();
        current_key_state = KEY_STATE_KEY1_ADD; // ��������ģʽ
    }
    else if (HAL_GPIO_ReadPin(GPIOC, KEY2_Pin) == RESET) {
        last_tick_time = xTaskGetTickCount();
        current_key_state = KEY_STATE_KEY2_CONFIG; // �������/����ģʽ
    }
    }
}


/**
 * @brief KEY1����ģʽ��������LED����˸�����ÿһ������50ms
 */
static void state_handler_key1_add(void)
{
    // ��鰴���Ƿ��ͷţ��̰�ȷ�ϣ�
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
 * @brief KEY2����ģʽ�����̰�����KEY2����ģʽ��������3s���뱣������ģʽ����
 */
static void state_handler_key2_config(void)
{
    // ��鰴���Ƿ��ͷ�
    if (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == SET) {
        // �̰��������ģʽ
        if (TIME_AFTER(xTaskGetTickCount(), last_tick_time, 20) && !TIME_AFTER(xTaskGetTickCount(), last_tick_time, 3000)) {
            current_key_state = KEY_STATE_KEY2_MINUS;
            printf("KEY2�̰�,�������ģʽ\n");
        }
        // ������ʱ���뱣������ģʽ
        else {
            current_key_state = KEY_STATE_SAVE_CONFIG;
        }
    }
    // ����Ƿ񳤰�
    else {
        if (TIME_AFTER(xTaskGetTickCount(), last_tick_time, 3000)) {
            current_key_state = KEY_STATE_SAVE_CONFIG;
        }
    }
}

/**
 * @brief KEY2����ģʽ��������LED����˸�����ÿһ�μ���50ms
 */
static void state_handler_key2_minus(void)
{
    // ȷ���ͷ�ʱ����20 < tick < 3000 ��Χ�ڣ�ִ�м���ģʽ
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
 * @brief ��������ģʽ����
 */
static void state_handler_save_config(void)
{
    KeyEvent_SendMode(LED_MODE_OFF);
    if (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == SET) {
        printf("����ɹ������صȴ�����״̬��");
        threshold_set = 0;
        current_key_state = KEY_STATE_WAIT;
    }
}

