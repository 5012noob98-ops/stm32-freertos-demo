#include "spi_oled_task.h"



static TaskHandle_t SpiOledTaskHandle = NULL;
extern QueueHandle_t xOLEDQueue;



void Spi_Oled_Task_Init(void){
    
    sh1106_init();
    sh1106_clear_screen();  


    BaseType_t xReturned;
    xReturned = xTaskCreate(  SPI_OLED_TASK,
                "OLED_TASK1",
                256,
                NULL,
                1,
                &SpiOledTaskHandle);
    if (xReturned == pdPASS)
    {
        LOG_INFO("[OLED] 任务1创建成功!\r\n");      
    }
}

void SPI_OLED_TASK(void *argument){
    
    static char str[100];
    SensorData_t Write_data ,OLED_data;
 

    while (1)
    {   
        if (xQueueReceive(xOLEDQueue, &Write_data, portMAX_DELAY) == pdPASS)
        {    
            
            OLED_data = Write_data;

            sh1106_clear_line(7);  
            sprintf(str, "Temp = %.1f", OLED_data.temperature);
            sh1106_show_string(7,1,str);

            sh1106_clear_line(6);  
            sprintf(str, "Humi = %.1f", OLED_data.humidity);
            sh1106_show_string(6,1,str);

        }
        sh1106_refresh_gram();
        vTaskDelay(100);              
        
    }
    
}




