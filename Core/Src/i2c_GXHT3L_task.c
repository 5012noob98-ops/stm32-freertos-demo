#include "i2c_GXHT3L_task.h"
#include "i2c_GXHT3L.h"
#include "task.h"
#include "main.h"
#include "queue.h"


/* 变量声明 */
static TaskHandle_t GXHT3LtaskHandle = NULL;
static uint8_t recv_dat[6] = {0};
static uint8_t recv_status[3] = {0};
QueueHandle_t xOLEDQueue;
QueueHandle_t ModbusRegUpdateQueue;



void OLEDEvent_SendMode(SensorData_t data) 
{
    xQueueSend(xOLEDQueue, &data, 0);
}



void i2c_GXHT3L_task_init(void){
    
    BaseType_t xReturned;
    
    GXHT3L_Reset();

    if(GXHT3L_Init() == HAL_OK)
        LOG_INFO("GXHT3L init ok.\n");
    else
        LOG_ERROR("GXHT3L init fail.\n");
    if(GXHT3L_Read_Status(recv_status) == HAL_OK){
        LOG_INFO("GXHT3L Read Status ok. Status = 0x%x%x\n",recv_status[0],recv_status[1]);
    }else{
        LOG_ERROR("GXHT3L Read Status fail.\n");
    }

    
    xReturned = xTaskCreate(  GXHT3L_READ_Task,
                "GXHT3L_task1",
                256,
                NULL,
                1,
                &GXHT3LtaskHandle);
    if (xReturned == pdPASS)
    {
        LOG_INFO("[GXHT3L] 任务1创建成功!\r\n");      
    }

    xOLEDQueue = xQueueCreate(5, sizeof(SensorData_t));
    ModbusRegUpdateQueue = xQueueCreate(10, sizeof(ModbusRegUpdateMsg_t));


}


void GXHT3L_READ_Task(void *argument){

    SensorData_t Read_data = {0.0f, 0.0f};

    while (1)
    {
        
        if(GXHT3L_Read_Dat(recv_dat) == HAL_OK)
        {
            if(GXHT3L_Dat_To_Float(recv_dat, &Read_data.temperature, &Read_data.humidity)==0)
            {

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

