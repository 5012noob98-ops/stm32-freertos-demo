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
 * @brief °´¼ü×´Ì¬Ã¶¾Ù
 */
typedef enum{
    
    KEY_STATE_WAIT,              ///< µÈ´ý°´¼ü×´Ì¬
    KEY_STATE_SHORT_PRESS,       ///< ¶Ì°´×´Ì¬
    KEY_STATE_LONG_PRESS,        ///< ³¤°´×´Ì¬
    KEY_STATE_DOUBLE_WAIT,       ///< µÈ´ýË«»÷×´Ì¬
    KEY_STATE_BLINKING,          ///< ÉÁË¸×´Ì¬
    KEY_STATE_CONFIG_WAIT,       ///< µÈ´ýÅäÖÃ×´Ì¬
    KEY_STATE_CONFIG_MODE,       ///< ÅäÖÃÄ£Ê½×´Ì¬
    KEY_STATE_KEY1_ADD,          ///< KEY1Ôö¼ÓÄ£Ê½
    KEY_STATE_KEY2_MINUS,        ///< KEY2¼õÉÙÄ£Ê½
    KEY_STATE_KEY2_CONFIG,       ///< KEY2ÅäÖÃÄ£Ê½
    KEY_STATE_SAVE_CONFIG        ///< ±£´æÅäÖÃ×´Ì¬

}KeyState;


/* º¯ÊýÉùÃ÷ */
void key_init(void);
void key_press_handler(void);
void reset_key_state(void);
KeyState get_key_state(void);


#endif
