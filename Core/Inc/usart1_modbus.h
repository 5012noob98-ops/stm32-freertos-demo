#ifndef USART1_MODBUS_H
#define USART1_MODBUS_H

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "led.h"
#include "led_task.h"
#include "usart.h"
#include "string.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"

#define SLAVE_ID 1

extern SemaphoreHandle_t xModbusRegMutex;
/* º¯ÊýÉùÃ÷ */
uint16_t Modbus_CRC16(uint8_t *buf,uint16_t len);
void Modbus_Process(uint8_t *rxBuf,uint16_t len);





#endif

