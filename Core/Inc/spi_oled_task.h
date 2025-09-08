#ifndef SPI_OLED_TASK_H
#define SPI_OLED_TASK_H

#include "spi_oled.h"
#include "FreeRTOS.h"
#include "queue.h"



/* º¯ÊýÉùÃ÷ */
void Spi_Oled_Task_Init(void);
void SPI_OLED_TASK(void *argument);


#endif

