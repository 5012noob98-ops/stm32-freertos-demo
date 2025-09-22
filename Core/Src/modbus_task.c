#include "modbus_task.h"
#include "main.h"

#define BUFFER_SIZE  256 // ���ջ�������С

/* ˽�б��� */
static TaskHandle_t ModbusTaskHandle = NULL;        // Modbus����������
static TaskHandle_t RegUpdateTaskHandle = NULL;     // �Ĵ�������������
static uint8_t rxBuf[BUFFER_SIZE];                  // USART���ջ�����
static volatile uint16_t rxLen = 0;                 // �������ݳ���
static SemaphoreHandle_t xModbusSemaphore;          // Modbus�ź���
static uint8_t localBuf[BUFFER_SIZE];               // �������ݻ�����

/* �ⲿ�������� */
extern QueueHandle_t ModbusRegUpdateQueue;          // �ⲿ�����ļĴ������¶���
extern uint16_t Modbus_HoldingReg[4];               // �ⲿ������Modbus���ּĴ���
extern DMA_HandleTypeDef hdma_usart1_rx;            // �ⲿ������USART1 DMA���

SemaphoreHandle_t xModbusRegMutex;                  // Modbus�Ĵ��������ź���

/**
 * @brief USART1 Modbus�����жϴ�����
 * 
 * ����USART1������ɺ�Ŀ����жϣ�����Modbus����֡����
 */
void  USART1_MODBUS_IDLEHandler(){

    if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) == SET) // �жϴ����Ƿ��ڿ���״̬
    {
        __HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_IDLE);     // �������״̬��־

        // ������յ������ݳ��� ���ճ���=�ܻ���������-ʣ������ճ���
        uint16_t len1 = BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);

        HAL_UART_DMAStop(&huart1); //�ر�DMA����

        if (len1 > 0 && len1 <= BUFFER_SIZE) 
        {
            rxLen = len1;  
            memcpy(localBuf, rxBuf, rxLen);            
        }    

        HAL_UART_Receive_DMA(&huart1, rxBuf, BUFFER_SIZE);
            
        // ֪ͨ��������
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(xModbusSemaphore, &xHigherPriorityTaskWoken);  
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }  
}

/**
 * @brief ��ʼ��Modbus����
 * 
 * ����Modbus����������Դ
 */
void Modbus_Task_init(void){
    
    xModbusSemaphore = xSemaphoreCreateBinary();//�����������ź�������ʼֵΪ0�����ֵ1
    xModbusRegMutex = xSemaphoreCreateMutex();  // ���������ź���
    
    BaseType_t xReturned;
    
    // ����Modbus��������
    xReturned = xTaskCreate(Modbus_Control_Task,
                            "Modbus_task1",
                            512,
                            NULL,
                            2,
                            &ModbusTaskHandle);
    if (xReturned == pdPASS)
    {
        LOG_INFO("[Modbus] ����1�����ɹ�!\r\n");      
    }

     // ����Modbus״̬�Ĵ�����������
    xReturned = xTaskCreate(Modbus_RegUpdate_Task,
                            "Modbus_task2",
                            256,
                            NULL,
                            1,
                            &RegUpdateTaskHandle);
    if (xReturned == pdPASS)
    {
        LOG_INFO("[Modbus] ����2�����ɹ�!\r\n");
    }

    // ʹ��UART�����жϲ�����DMA����
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
    HAL_UART_Receive_DMA(&huart1,rxBuf,sizeof(rxBuf));
}

/**
 * @brief Modbus��������
 * 
 * ����Modbusͨ�ſ����߼�
 * @param argument �������
 */
void Modbus_Control_Task(void *argument){


    while (1)
    {
        // ��ȡ�ź���
        if (xSemaphoreTake(xModbusSemaphore,portMAX_DELAY))
        {
            HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin); // �л�LED״̬�Թ۲�����ִ��
            if(rxLen > 0) {
                uint16_t len2 = rxLen;
                if(len2 > BUFFER_SIZE) len2 = BUFFER_SIZE; // ���ռ�飬��ֹ���

                taskENTER_CRITICAL();
                rxLen = 0;
                taskEXIT_CRITICAL();

                // ����ModbusЭ��
                Modbus_Process(localBuf,len2);
            }
        }
    }  
}

/**
 * @brief Modbus״̬�Ĵ�����������
 * 
 * ������������ģ��ļĴ�����������
 * @param argument �������
 */
void Modbus_RegUpdate_Task(void *argument){

    ModbusRegUpdateMsg_t Write_msg;

    while (1)
    {
        // �Ӷ��н��ռĴ���������Ϣ
        if (xQueueReceive(ModbusRegUpdateQueue, &Write_msg, portMAX_DELAY) == pdPASS) {
            if (Write_msg.addr < 4) {
                // ʹ�û�������������
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
//         HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin); // �л�LED״̬�Թ۲�����ִ��

//     }
// }

