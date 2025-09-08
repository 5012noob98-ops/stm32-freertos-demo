#ifndef SPI_OLED_H
#define SPI_OLED_H

#include "main.h"
#include <stdio.h>
#include "spi.h"
#include "font.h"
#include "i2c_GXHT3L_task.h"
#include "i2c_GXHT3L.h"
#include "log.h"


/* º¯ÊýÉùÃ÷ */
static void sh1106_reset(void); //ÆÁÄ»¸´Î»
static void sh1106_write_cmd(uint8_t chData);
static void sh1106_write_data(uint8_t chData);
void sh1106_clear_screen(void);
void sh1106_draw_point(uint8_t chXpos, uint8_t chYpos, uint8_t chPoint);
void sh1106_refresh_gram(void);
void sh1106_draw_bitmap(uint8_t chXpos, uint8_t chYpos, const uint8_t *pchBmp, uint8_t chWidth, uint8_t chHeight);
void sh1106_set_cursor(uint8_t Y, uint8_t X);
void sh1106_show_char(uint8_t Line, uint8_t Column, char Char);
void sh1106_show_string(uint8_t Line, uint8_t Column, char *String);
void sh1106_init(void);
void sh1106_clear_line(uint8_t page) ;


#endif

