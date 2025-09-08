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
  

/* �������� */
void i2c_GXHT3L_task_init(void); //GXHT3L�����ʼ��
void GXHT3L_READ_Task(void *argument);//GXHT3L�ɼ���ת����ʪ��
void OLEDEvent_SendMode(SensorData_t data) ;




#endif

