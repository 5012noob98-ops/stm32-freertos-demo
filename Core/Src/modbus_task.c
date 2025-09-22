#include "modbus_task.h"
#include "main.h"

#define BUFFER_SIZE  256 // 接收缓冲区大小

/* 私有变量 */
static TaskHandle_t ModbusTaskHandle = NULL;        // Modbus控制任务句柄
static TaskHandle_t RegUpdateTaskHandle = NULL;     // 寄存器更新任务句柄
static uint8_t rxBuf[BUFFER_SIZE];                  // USART接收缓冲区
static volatile uint16_t rxLen = 0;                 // 接收数据长度
static SemaphoreHandle_t xModbusSemaphore;          // Modbus信号量
static uint8_t localBuf[BUFFER_SIZE];               // 本地数据缓冲区

/* 外部变量声明 */
extern QueueHandle_t ModbusRegUpdateQueue;          // 外部声明的寄存器更新队列
extern uint16_t Modbus_HoldingReg[4];               // 外部声明的Modbus保持寄存器
extern DMA_HandleTypeDef hdma_usart1_rx;            // 外部声明的USART1 DMA句柄

SemaphoreHandle_t xModbusRegMutex;                  // Modbus寄存器互斥信号量

/**
 * @brief USART1 Modbus空闲中断处理函数
 * 
 * 处理USART1接收完成后的空闲中断，用于Modbus数据帧接收
 */
void  USART1_MODBUS_IDLEHandler(){

    if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) == SET) // 判断串口是否处于空闲状态
    {
        __HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_IDLE);     // 清除空闲状态标志

        // 计算接收到的数据长度 接收长度=总缓冲区长度-剩余待接收长度
        uint16_t len1 = BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);

        HAL_UART_DMAStop(&huart1); //关闭DMA传输

        if (len1 > 0 && len1 <= BUFFER_SIZE) 
        {
            rxLen = len1;  
            memcpy(localBuf, rxBuf, rxLen);            
        }    

        HAL_UART_Receive_DMA(&huart1, rxBuf, BUFFER_SIZE);
            
        // 通知处理任务
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(xModbusSemaphore, &xHigherPriorityTaskWoken);  
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }  
}

/**
 * @brief 初始化Modbus任务
 * 
 * 创建Modbus相关任务和资源
 */
void Modbus_Task_init(void){
    
    xModbusSemaphore = xSemaphoreCreateBinary();//创建二进制信号量，初始值为0，最大值1
    xModbusRegMutex = xSemaphoreCreateMutex();  // 创建互斥信号量
    
    BaseType_t xReturned;
    
    // 创建Modbus控制任务
    xReturned = xTaskCreate(Modbus_Control_Task,
                            "Modbus_task1",
                            512,
                            NULL,
                            2,
                            &ModbusTaskHandle);
    if (xReturned == pdPASS)
    {
        LOG_INFO("[Modbus] 任务1创建成功!\r\n");      
    }

     // 创建Modbus状态寄存器更新任务
    xReturned = xTaskCreate(Modbus_RegUpdate_Task,
                            "Modbus_task2",
                            256,
                            NULL,
                            1,
                            &RegUpdateTaskHandle);
    if (xReturned == pdPASS)
    {
        LOG_INFO("[Modbus] 任务2创建成功!\r\n");
    }

    // 使能UART空闲中断并启动DMA接收
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
    HAL_UART_Receive_DMA(&huart1,rxBuf,sizeof(rxBuf));
}

/**
 * @brief Modbus控制任务
 * 
 * 处理Modbus通信控制逻辑
 * @param argument 任务参数
 */
void Modbus_Control_Task(void *argument){


    while (1)
    {
        // 获取信号量
        if (xSemaphoreTake(xModbusSemaphore,portMAX_DELAY))
        {
            HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin); // 切换LED状态以观察任务执行
            if(rxLen > 0) {
                uint16_t len2 = rxLen;
                if(len2 > BUFFER_SIZE) len2 = BUFFER_SIZE; // 保险检查，防止溢出

                taskENTER_CRITICAL();
                rxLen = 0;
                taskEXIT_CRITICAL();

                // 处理Modbus协议
                Modbus_Process(localBuf,len2);
            }
        }
    }  
}

/**
 * @brief Modbus状态寄存器更新任务
 * 
 * 处理来自其他模块的寄存器更新请求
 * @param argument 任务参数
 */
void Modbus_RegUpdate_Task(void *argument){

    ModbusRegUpdateMsg_t Write_msg;

    while (1)
    {
        // 从队列接收寄存器更新消息
        if (xQueueReceive(ModbusRegUpdateQueue, &Write_msg, portMAX_DELAY) == pdPASS) {
            if (Write_msg.addr < 4) {
                // 使用互斥量保护访问
                if (xSemaphoreTake(xModbusRegMutex, portMAX_DELAY)) {
                    Modbus_HoldingReg[Write_msg.addr] = Write_msg.value;
                    xSemaphoreGive(xModbusRegMutex);
                }
            }
        }
    }  
}


// void vModbusTimerCallback(TimerHandle_t xTimer){
    
//     if(rxLen > 0) {
//         HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin); // 切换LED状态以观察任务执行

//     }
// }

