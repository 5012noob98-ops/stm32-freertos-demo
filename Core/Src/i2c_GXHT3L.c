#include "i2c_GXHT3L.h"


/*  宏定义  */
#define    GXHT3L_ADDR_WRITE    0x44<<1         //10001000 
#define    GXHT3L_ADDR_READ     (0x44<<1)+1     //10001001
#define    CRC8_POLYNOMIAL      0x31

/**
 * @brief    向GXHT3L发送一条指令(16bit)
 * @param    cmd —— GXHT3L指令（在GXHT3L_MODE中枚举定义）
 * @retval    成功返回HAL_OK
*/
uint8_t GXHT3L_Send_Cmd(GXHT3L_CMD cmd)
{
    uint8_t cmd_buffer[2];
    cmd_buffer[0] = cmd >> 8;
    cmd_buffer[1] = cmd;
    return HAL_I2C_Master_Transmit(&hi2c2, GXHT3L_ADDR_WRITE, (uint8_t*) cmd_buffer, 2, 0xFFFF);
}

/**
 * @brief    复位GXHT3L
 * @param    none
 * @retval    none
*/
void GXHT3L_Reset(void)
{
    GXHT3L_Send_Cmd(SOFT_RESET_CMD);
    vTaskDelay(10);      

}

/**
 * @brief    禁用GXHT3L的加热
 * @param    none
 * @retval    none
*/
void GXHT3L_Preheat_Disable(void)
{
    GXHT3L_Send_Cmd(PREHEAT_DISENABLE_CMD);
    vTaskDelay(10);      

}

/**
 * @brief    读取GXHT3L内部状态寄存器
 * @param    dat —— 存放接收读取的数据
 * @retval    成功返回HAL_OK
*/
uint8_t GXHT3L_Read_Status(uint8_t* dat)
{
    GXHT3L_Send_Cmd(DEVICE_STATUS_CMD);
    vTaskDelay(10);      
    return HAL_I2C_Master_Receive(&hi2c2, GXHT3L_ADDR_READ, dat, 3, 0xFFFF);
}

/**
 * @brief    初始化GXHT3L
 * @param    none
 * @retval    成功返回HAL_OK
 * @note    周期测量模式
*/
uint8_t GXHT3L_Init(void)
{
    return GXHT3L_Send_Cmd(MEDIUM_2_CMD);//每2s测量一次，重复率中
    
}

/**
 * @brief    从GXHT3L读取一次数据
 * @param    dat —— 存储读取数据的地址（6个字节数组）
 * @retval    成功 —— 返回HAL_OK
*/
uint8_t GXHT3L_Read_Dat(uint8_t* dat)
{
    GXHT3L_Send_Cmd(READOUT_FOR_PERIODIC_MODE);
    return HAL_I2C_Master_Receive(&hi2c2, GXHT3L_ADDR_READ, dat, 6, 0xFFFF);
}

/**
 * @brief    CRC校验
 * @param    
 * @retval    
*/

uint8_t CheckCrc8(uint8_t* const message, uint8_t initial_value)
{
    uint8_t  remainder;        //余数
    uint8_t  i = 0, j = 0;  //循环变量

    /* 初始化 */
    remainder = initial_value;
    for(j = 0; j < 2;j++)
    {
        remainder ^= message[j];
        /* 从最高位开始依次计算  */
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
    /* 返回计算的CRC码 */
    return remainder;
}

/**
 * @brief    将GXHT3L接收的6个字节数据进行CRC校验，并转换为温度值和湿度值
 * @param    dat  —— 存储接收数据的地址（6个字节数组）
 * @retval    校验成功  —— 返回0
 *             校验失败  —— 返回1，并设置温度值和湿度值为0
*/
uint8_t GXHT3L_Dat_To_Float(uint8_t* const dat, float* temperature, float* humidity)
{
    uint16_t recv_temperature = 0;
    uint16_t recv_humidity = 0;

    /* 校验温度数据和湿度数据是否接收正确 */
    if(CheckCrc8(dat, 0xFF) != dat[2] || CheckCrc8(&dat[3], 0xFF) != dat[5])
        return 1;

    /* 转换温度数据 */
    recv_temperature = ((uint16_t)dat[0]<<8)|dat[1];
    *temperature = -45 + 175*((float)recv_temperature/65535);

    /* 转换湿度数据 */
    recv_humidity = ((uint16_t)dat[3]<<8)|dat[4];
    *humidity = 100 * ((float)recv_humidity / 65535);

    return 0;
}

