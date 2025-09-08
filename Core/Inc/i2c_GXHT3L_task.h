#ifndef I2C_GXHT3L_TASK_H
#define I2C_GXHT3L_TASK_H

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "i2c.h"
#include "i2c_GXHT3L.h"


typedef struct {
    float temperature;
    float humidity;
} SensorData_t;

typedef struct{
    uint16_t addr;
    uint16_t value;
} ModbusRegUpdateMsg_t;
  

/* 函数声明 */
void i2c_GXHT3L_task_init(void); //GXHT3L任务初始化
void GXHT3L_READ_Task(void *argument);//GXHT3L采集并转换温湿度
void OLEDEvent_SendMode(SensorData_t data) ;




#endif

