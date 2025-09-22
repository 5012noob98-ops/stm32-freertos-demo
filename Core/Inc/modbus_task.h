#ifndef MODBUS_TASK_H
#define MODBUS_TASK_H

#include <stdio.h>
#include "usart1_modbus.h"
#include "task.h"
#include "queue.h"
#include "log.h"
#include "semphr.h"
#include "FreeRTOS.h"
#include "i2c_GXHT3L_task.h"

/* º¯ÊýÉùÃ÷ */

void USART1_MODBUS_IDLEHandler(void);
void Modbus_Task_init(void);
void Modbus_Control_Task(void *argument);
void Modbus_RegUpdate_Task(void *argument);

// void vModbusTimerCallback(TimerHandle_t xTimer);

#endif
