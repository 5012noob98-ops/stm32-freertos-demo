#include "i2c_GXHT3L_task.h"
#include "i2c_GXHT3L.h"
#include "task.h"
#include "main.h"
#include "queue.h"


/* 私有变量 */
static TaskHandle_t GXHT3LtaskHandle = NULL;           // GXHT3L任务句柄
static uint8_t recv_dat[6] = {0};                      // 接收传感器数据缓冲区
static uint8_t recv_status[3] = {0};                   // 接收内部状态寄存器数据缓冲区

QueueHandle_t xOLEDQueue;                              // OLED显示队列句柄
QueueHandle_t ModbusRegUpdateQueue;                    // Modbus寄存器更新队列句柄

/**
 * @brief 发送OLED显示事件
 * 
 * 将传感器数据通过队列发送给OLED显示任务
 * @param data 传感器转换的温湿度数据
 */
void OLEDEvent_SendMode(SensorData_t data) 
{
    xQueueSend(xOLEDQueue, &data, 0);
}


/**
 * @brief 初始化GXHT3L传感器任务
 * 
 * 初始化传感器，创建任务和相关队列
 */
void i2c_GXHT3L_task_init(void){ 
    BaseType_t xReturned;
    
    // 复位传感器
    GXHT3L_Reset();

    // 初始化传感器
    if(GXHT3L_Init() == HAL_OK)
        LOG_INFO("GXHT3L init ok.\n");
    else
        LOG_ERROR("GXHT3L init fail.\n");

    // 读取传感器状态，内部状态寄存器包含加热器状态，报警信息，复位信息，crc校验信息，命令执行情况。
    if(GXHT3L_Read_Status(recv_status) == HAL_OK){
        LOG_INFO("GXHT3L Read Status ok. Status = 0x%x%x\n",recv_status[0],recv_status[1]);
    }else{
        LOG_ERROR("GXHT3L Read Status fail.\n");
    }

    // 创建传感器读取任务
    xReturned = xTaskCreate(  GXHT3L_Read_Task,
                "GXHT3L_task1",
                256,
                NULL,
                1,
                &GXHT3LtaskHandle);
    if (xReturned == pdPASS)
    {
        LOG_INFO("[GXHT3L] 任务1创建成功!\r\n");      
    }

    // 创建队列
    xOLEDQueue = xQueueCreate(5, sizeof(SensorData_t));
    ModbusRegUpdateQueue = xQueueCreate(10, sizeof(ModbusRegUpdateMsg_t));


}

/**
 * @brief GXHT3L传感器数据读取任务
 * 
 * 周期性读取传感器数据并通过队列发送给其他任务
 * @param argument 任务参数
 */
void GXHT3L_Read_Task(void *argument){
    SensorData_t Read_data = {0.0f, 0.0f};

    while (1)
    {
        // 读取传感器数据
        if(GXHT3L_Read_Dat(recv_dat) == HAL_OK)
        {
            // 数据CRC校验和转换
            if(GXHT3L_Dat_To_Float(recv_dat, &Read_data.temperature, &Read_data.humidity)==0)
            {
                // 发送数据给OLED显示任务
                OLEDEvent_SendMode(Read_data);                
                LOG_INFO("temperature = %f, humidity = %f\n", Read_data.temperature, Read_data.humidity);

                // ===== 新增：写队列更新保持寄存器 =====
                ModbusRegUpdateMsg_t Read_msg;
                Read_msg.addr = 0; // 温度寄存器地址
                Read_msg.value = (int16_t)(Read_data.temperature * 10); 
                xQueueSend(ModbusRegUpdateQueue, &Read_msg, 0);

                Read_msg.addr = 1; // 湿度寄存器地址
                Read_msg.value = (int16_t)(Read_data.humidity * 10);
                xQueueSend(ModbusRegUpdateQueue, &Read_msg, 0);
            }
            else
            {
                LOG_ERROR("crc check fail.\n");
            }
        }
        else
        {
            LOG_ERROR("read data from GXHT3L fail.\n");
        }    
        vTaskDelay(2000);      
    }  
}
