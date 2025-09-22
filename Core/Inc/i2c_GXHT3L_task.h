#ifndef I2C_GXHT3L_TASK_H
#define I2C_GXHT3L_TASK_H

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "i2c.h"
#include "i2c_GXHT3L.h"

/**
 * @brief ���������ݽṹ��
 * 
 * ���ڴ洢��ʪ�ȴ�������ȡ������
 */
typedef struct {
    float temperature;  // �¶�ֵ
    float humidity;     // ʪ��ֵ
} SensorData_t;

/**
 * @brief Modbus�Ĵ���������Ϣ�ṹ��
 * 
 * ����ͨ�����д���Modbus�Ĵ���������Ϣ
 */
typedef struct{
    uint16_t addr;   // �Ĵ�����ַ
    uint16_t value;  // �Ĵ���ֵ
} ModbusRegUpdateMsg_t;
  
/* �������� */

void i2c_GXHT3L_task_init(void); //GXHT3L�����ʼ��
void GXHT3L_Read_Task(void *argument);//GXHT3L�ɼ���ת����ʪ��
void OLEDEvent_SendMode(SensorData_t data) ;

#endif
