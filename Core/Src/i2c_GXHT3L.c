#include "i2c_GXHT3L.h"


/*  �궨��  */
#define    GXHT3L_ADDR_WRITE    0x44<<1         //10001000 
#define    GXHT3L_ADDR_READ     (0x44<<1)+1     //10001001
#define    CRC8_POLYNOMIAL      0x31

/**
 * @brief    ��GXHT3L����һ��ָ��(16bit)
 * @param    cmd ���� GXHT3Lָ���GXHT3L_MODE��ö�ٶ��壩
 * @retval    �ɹ�����HAL_OK
*/
uint8_t GXHT3L_Send_Cmd(GXHT3L_CMD cmd)
{
    uint8_t cmd_buffer[2];
    cmd_buffer[0] = cmd >> 8;
    cmd_buffer[1] = cmd;
    return HAL_I2C_Master_Transmit(&hi2c2, GXHT3L_ADDR_WRITE, (uint8_t*) cmd_buffer, 2, 0xFFFF);
}

/**
 * @brief    ��λGXHT3L
 * @param    none
 * @retval    none
*/
void GXHT3L_Reset(void)
{
    GXHT3L_Send_Cmd(SOFT_RESET_CMD);
    vTaskDelay(10);      

}

/**
 * @brief    ����GXHT3L�ļ���
 * @param    none
 * @retval    none
*/
void GXHT3L_Preheat_Disable(void)
{
    GXHT3L_Send_Cmd(PREHEAT_DISENABLE_CMD);
    vTaskDelay(10);      

}

/**
 * @brief    ��ȡGXHT3L�ڲ�״̬�Ĵ���
 * @param    dat ���� ��Ž��ն�ȡ������
 * @retval    �ɹ�����HAL_OK
*/
uint8_t GXHT3L_Read_Status(uint8_t* dat)
{
    GXHT3L_Send_Cmd(DEVICE_STATUS_CMD);
    vTaskDelay(10);      
    return HAL_I2C_Master_Receive(&hi2c2, GXHT3L_ADDR_READ, dat, 3, 0xFFFF);
}

/**
 * @brief    ��ʼ��GXHT3L
 * @param    none
 * @retval    �ɹ�����HAL_OK
 * @note    ���ڲ���ģʽ
*/
uint8_t GXHT3L_Init(void)
{
    return GXHT3L_Send_Cmd(MEDIUM_2_CMD);//ÿ2s����һ�Σ��ظ�����
    
}

/**
 * @brief    ��GXHT3L��ȡһ������
 * @param    dat ���� �洢��ȡ���ݵĵ�ַ��6���ֽ����飩
 * @retval    �ɹ� ���� ����HAL_OK
*/
uint8_t GXHT3L_Read_Dat(uint8_t* dat)
{
    GXHT3L_Send_Cmd(READOUT_FOR_PERIODIC_MODE);
    return HAL_I2C_Master_Receive(&hi2c2, GXHT3L_ADDR_READ, dat, 6, 0xFFFF);
}

/**
 * @brief    CRCУ��
 * @param    
 * @retval    
*/

uint8_t CheckCrc8(uint8_t* const message, uint8_t initial_value)
{
    uint8_t  remainder;        //����
    uint8_t  i = 0, j = 0;  //ѭ������

    /* ��ʼ�� */
    remainder = initial_value;
    for(j = 0; j < 2;j++)
    {
        remainder ^= message[j];
        /* �����λ��ʼ���μ���  */
        for (i = 0; i < 8; i++)
        {
            if (remainder & 0x80)
            {
                remainder = (remainder << 1)^CRC8_POLYNOMIAL;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }
    }
    /* ���ؼ����CRC�� */
    return remainder;
}

/**
 * @brief    ��GXHT3L���յ�6���ֽ����ݽ���CRCУ�飬��ת��Ϊ�¶�ֵ��ʪ��ֵ
 * @param    dat  ���� �洢�������ݵĵ�ַ��6���ֽ����飩
 * @retval    У��ɹ�  ���� ����0
 *             У��ʧ��  ���� ����1���������¶�ֵ��ʪ��ֵΪ0
*/
uint8_t GXHT3L_Dat_To_Float(uint8_t* const dat, float* temperature, float* humidity)
{
    uint16_t recv_temperature = 0;
    uint16_t recv_humidity = 0;

    /* У���¶����ݺ�ʪ�������Ƿ������ȷ */
    if(CheckCrc8(dat, 0xFF) != dat[2] || CheckCrc8(&dat[3], 0xFF) != dat[5])
        return 1;

    /* ת���¶����� */
    recv_temperature = ((uint16_t)dat[0]<<8)|dat[1];
    *temperature = -45 + 175*((float)recv_temperature/65535);

    /* ת��ʪ������ */
    recv_humidity = ((uint16_t)dat[3]<<8)|dat[4];
    *humidity = 100 * ((float)recv_humidity / 65535);

    return 0;
}

