#ifndef I2C_GXHT3L_TASK_H
#define I2C_GXHT3L_TASK_H

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "i2c.h"
#include "i2c_GXHT3L.h"

/**
 * @brief 传感器数据结构体
 * 
 * 用于存储温湿度传感器读取的数据
 */
typedef struct {
    float temperature;  // 温度值
    float humidity;     // 湿度值
} SensorData_t;

/**
 * @brief Modbus寄存器更新消息结构体
 * 
 * 用于通过队列传递Modbus寄存器更新信息
 */
typedef struct{
    uint16_t addr;   // 寄存器地址
    uint16_t value;  // 寄存器值
} ModbusRegUpdateMsg_t;
  
/* 函数声明 */

void i2c_GXHT3L_task_init(void); //GXHT3L任务初始化
void GXHT3L_Read_Task(void *argument);//GXHT3L采集并转换温湿度
void OLEDEvent_SendMode(SensorData_t data) ;

#endif
