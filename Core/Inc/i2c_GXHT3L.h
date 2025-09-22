#ifndef I2C_GXHT3L_H
#define I2C_GXHT3L_H

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "i2c.h"
#include "log.h"

/* 函数变量 */

/**
 * @brief GXHT3L传感器命令枚举定义
 */
typedef enum
{
    /* 软件复位命令 */
    SOFT_RESET_CMD = 0x30A2,    

    /* 加热使能/禁能命令 */
    PREHEAT_ENABLE_CMD = 0x306D,
    PREHEAT_DISENABLE_CMD = 0x3066,

    /* 芯片状态命令 */
    DEVICE_STATUS_CMD = 0xF32D,
    
    /*
    单次测量模式
    命名格式：Repeatability_CS_CMD
    CS：Clock stretching
    */
    HIGH_ENABLED_CMD    = 0x2C06,
    MEDIUM_ENABLED_CMD  = 0x2C0D,
    LOW_ENABLED_CMD     = 0x2C10,
    HIGH_DISABLED_CMD   = 0x2400,
    MEDIUM_DISABLED_CMD = 0x240B,
    LOW_DISABLED_CMD    = 0x2416,

    /*
    周期测量模式
    命名格式：Repeatability_MPS_CMD
    MPS：measurement per second
    */
    HIGH_0_5_CMD   = 0x2032,
    MEDIUM_0_5_CMD = 0x2024,
    LOW_0_5_CMD    = 0x202F,
    HIGH_1_CMD     = 0x2130,
    MEDIUM_1_CMD   = 0x2126,
    LOW_1_CMD      = 0x212D,
    HIGH_2_CMD     = 0x2236,
    MEDIUM_2_CMD   = 0x2220,
    LOW_2_CMD      = 0x222B,
    HIGH_4_CMD     = 0x2334,
    MEDIUM_4_CMD   = 0x2322,
    LOW_4_CMD      = 0x2329,
    HIGH_10_CMD    = 0x2737,
    MEDIUM_10_CMD  = 0x2721,
    LOW_10_CMD     = 0x272A,

    /* 周期测量模式读取数据命令 */
    READOUT_FOR_PERIODIC_MODE = 0xE000,
} GXHT3L_CMD;

/* 函数声明 */

uint8_t GXHT3L_Init(void);                                          //初始化GXHT3L
void GXHT3L_Reset(void);                                            //复位GXHT3L
void GXHT3L_Preheat_Disable(void);                                  //禁用GXHT3L的加热
uint8_t GXHT3L_Read_Status(uint8_t* dat);                           //读取GXHT3L内部状态寄存器
uint8_t GXHT3L_Read_Dat(uint8_t* dat);                              //从GXHT3L读取一次数据
uint8_t GXHT3L_Dat_To_Float(uint8_t* const dat, float* temperature, float* humidity);//将GXHT3L接收的6个字节数据进行CRC校验，并转换为温度值和湿度值

#endif
