#ifndef I2C_GXHT3L_H
#define I2C_GXHT3L_H

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "i2c.h"
#include "log.h"

/* �������� */

/**
 * @brief GXHT3L����������ö�ٶ���
 */
typedef enum
{
    /* �����λ���� */
    SOFT_RESET_CMD = 0x30A2,    

    /* ����ʹ��/�������� */
    PREHEAT_ENABLE_CMD = 0x306D,
    PREHEAT_DISENABLE_CMD = 0x3066,

    /* оƬ״̬���� */
    DEVICE_STATUS_CMD = 0xF32D,
    
    /*
    ���β���ģʽ
    ������ʽ��Repeatability_CS_CMD
    CS��Clock stretching
    */
    HIGH_ENABLED_CMD    = 0x2C06,
    MEDIUM_ENABLED_CMD  = 0x2C0D,
    LOW_ENABLED_CMD     = 0x2C10,
    HIGH_DISABLED_CMD   = 0x2400,
    MEDIUM_DISABLED_CMD = 0x240B,
    LOW_DISABLED_CMD    = 0x2416,

    /*
    ���ڲ���ģʽ
    ������ʽ��Repeatability_MPS_CMD
    MPS��measurement per second
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

    /* ���ڲ���ģʽ��ȡ�������� */
    READOUT_FOR_PERIODIC_MODE = 0xE000,
} GXHT3L_CMD;

/* �������� */

uint8_t GXHT3L_Init(void);                                          //��ʼ��GXHT3L
void GXHT3L_Reset(void);                                            //��λGXHT3L
void GXHT3L_Preheat_Disable(void);                                  //����GXHT3L�ļ���
uint8_t GXHT3L_Read_Status(uint8_t* dat);                           //��ȡGXHT3L�ڲ�״̬�Ĵ���
uint8_t GXHT3L_Read_Dat(uint8_t* dat);                              //��GXHT3L��ȡһ������
uint8_t GXHT3L_Dat_To_Float(uint8_t* const dat, float* temperature, float* humidity);//��GXHT3L���յ�6���ֽ����ݽ���CRCУ�飬��ת��Ϊ�¶�ֵ��ʪ��ֵ

#endif
