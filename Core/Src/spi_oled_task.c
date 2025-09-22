#include "spi_oled_task.h"

/* ˽�б��� */

static TaskHandle_t SpiOledTaskHandle = NULL;
extern QueueHandle_t xOLEDQueue;

/**
 * @brief ��ʼ��SPI OLED����
 * 
 * ��ʼ��OLED��ʾ��������OLED��ʾ����
 */
void Spi_Oled_Task_Init(void){
     // ��ʼ��OLED��ʾ��
    sh1106_init();
    sh1106_clear_screen();
		sh1106_refresh_gram();

    BaseType_t xReturned;
    // ����OLED��ʾ����
    xReturned = xTaskCreate(  SPI_OLED_TASK,
                "OLED_TASK1",
                256,
                NULL,
                1,
                &SpiOledTaskHandle);
    if (xReturned == pdPASS)
    {
        LOG_INFO("[OLED] ����1�����ɹ�!\r\n");      
    }
}

/**
 * @brief SPI OLED��ʾ����
 * 
 * �Ӷ��н��մ��������ݲ���OLED����ʾ�¶Ⱥ�ʪ����Ϣ
 * @param argument �������
 */
void SPI_OLED_TASK(void *argument){
    
    static char str[100];   // �ַ���������
    SensorData_t Write_data ,OLED_data; // ���������ݱ���
 
		vTaskDelay(100);   
    while (1)
    {   
			
        // �Ӷ��н��մ���������
        if (xQueueReceive(xOLEDQueue, &Write_data, portMAX_DELAY) == pdPASS)
        {    
            
            OLED_data = Write_data;

            // �����7�в���ʾ�¶���Ϣ
            sh1106_clear_line(7);  
            sprintf(str, "Temp = %.1f", OLED_data.temperature);
            sh1106_show_string(7,1,str);

            // �����6�в���ʾʪ����Ϣ
            sh1106_clear_line(6);  
            sprintf(str, "Humi = %.1f", OLED_data.humidity);
            sh1106_show_string(6,1,str);
        }

        // ˢ����ʾ����
				sh1106_refresh_gram();
        vTaskDelay(100);                
    }
}
