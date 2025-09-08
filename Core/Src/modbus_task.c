#include "modbus_task.h"

#define BUFFER_SIZE  256

static TaskHandle_t ModbusTaskHandle = NULL;
static TaskHandle_t RegUpdateTaskHandle = NULL;
// QueueHandle_t xModbusQueue;
static uint8_t rxBuf[BUFFER_SIZE];
static volatile uint16_t rxLen = 0;
static SemaphoreHandle_t xModbusSemaphore;
// static TimerHandle_t xModbusTimer;
static uint8_t localBuf[BUFFER_SIZE];
extern QueueHandle_t ModbusRegUpdateQueue;
extern uint16_t Modbus_HoldingReg[4];
SemaphoreHandle_t xModbusRegMutex;
extern DMA_HandleTypeDef hdma_usart1_rx;

void  USART1_MODBUS_IDLEHandler(){

    // BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // xTimerStartFromISR(xModbusTimer, &xHigherPriorityTaskWoken);
    // portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

    if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) == SET) //如果串口处于空闲状态
    {
        __HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_IDLE);//清空空闲状态标志

        //计算接收到的数据长度 ，已接收长度=需要接收总长度-剩余待接收长度
        uint16_t len1 = BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
        // rxLen = BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
        HAL_UART_DMAStop(&huart1); //关闭DMA传输
        if (len1 > 0 && len1 <= BUFFER_SIZE) 
        {
            rxLen = len1;  
            memcpy(localBuf, rxBuf, rxLen);            
        }    // rxLen 应声明为 volatile
        HAL_UART_Receive_DMA(&huart1, rxBuf, BUFFER_SIZE);
            // 4) 通知任务处理（FromISR 版本）
            // vTaskNotifyGiveFromISR(ModbusTaskHandle, &xHigherPriorityTaskWoken);
            // portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(xModbusSemaphore, &xHigherPriorityTaskWoken);  
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    
}




void Modbus_Task_init(void){
    
    // xModbusQueue = xQueueCreate(5, sizeof(LED_Event_t));
    xModbusSemaphore = xSemaphoreCreateBinary();//创建二进制信号量，初始值为0，最大值1
    xModbusRegMutex = xSemaphoreCreateMutex();
    
    BaseType_t xReturned;
    // size_t freeHeapBeforeTask1 = xPortGetFreeHeapSize();
    
    xReturned = xTaskCreate(  Modbus_Control_Task,
        "Modbus_task1",
                512,
                NULL,
                2,
                &ModbusTaskHandle);
    if (xReturned == pdPASS)
    {
        LOG_INFO("[Modbus] 任务1创建成功!\r\n");      
    }



    xReturned = xTaskCreate(  Modbus_RegUpdate_Task,
                "Modbus_task2",
                256,
                NULL,
                1,
                &RegUpdateTaskHandle);
    if (xReturned == pdPASS)
    {
        LOG_INFO("[Modbus] 任务2创建成功!\r\n");
    }

    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
    HAL_UART_Receive_DMA(&huart1,rxBuf,sizeof(rxBuf));

    // xModbusTimer = xTimerCreate("ModbusTimer",
    //                             5,
    //                             pdFALSE,
    //                             NULL,
    //                             vModbusTimerCallback);
}

void Modbus_Control_Task(void *argument){


    while (1)
    {
        if (xSemaphoreTake(xModbusSemaphore,portMAX_DELAY))//获取信号量
        {
            HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin); // 切换LED状态以观察任务执行
            if(rxLen > 0) {
                uint16_t len2 = rxLen;
                if(len2 > BUFFER_SIZE) len2 = BUFFER_SIZE; // 保险检查

                taskENTER_CRITICAL();
                // memcpy(localBuf, rxBuf, len2);    
                rxLen = 0;
                taskEXIT_CRITICAL();
                
                // HAL_UART_Transmit(&huart1, localBuf, len2, 100);

                Modbus_Process(localBuf,len2);
            }
        }
    }  
}

void Modbus_RegUpdate_Task(void *argument){

    ModbusRegUpdateMsg_t Write_msg;

    while (1)
    {
        if (xQueueReceive(ModbusRegUpdateQueue, &Write_msg, portMAX_DELAY) == pdPASS) {
            if (Write_msg.addr < 4) {
                // Modbus_HoldingReg[Write_msg.addr] = Write_msg.value;
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

