#include "spi_oled_task.h"

/* 私有变量 */

static TaskHandle_t SpiOledTaskHandle = NULL;
extern QueueHandle_t xOLEDQueue;

/**
 * @brief 初始化SPI OLED任务
 * 
 * 初始化OLED显示屏并创建OLED显示任务
 */
void Spi_Oled_Task_Init(void){
     // 初始化OLED显示屏
    sh1106_init();
    sh1106_clear_screen();
		sh1106_refresh_gram();

    BaseType_t xReturned;
    // 创建OLED显示任务
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

/**
 * @brief SPI OLED显示任务
 * 
 * 从队列接收传感器数据并在OLED上显示温度和湿度信息
 * @param argument 任务参数
 */
void SPI_OLED_TASK(void *argument){
    
    static char str[100];   // 字符串缓冲区
    SensorData_t Write_data ,OLED_data; // 传感器数据变量
 
		vTaskDelay(100);   
    while (1)
    {   
			
        // 从队列接收传感器数据
        if (xQueueReceive(xOLEDQueue, &Write_data, portMAX_DELAY) == pdPASS)
        {    
            
            OLED_data = Write_data;

            // 清除第7行并显示温度信息
            sh1106_clear_line(7);  
            sprintf(str, "Temp = %.1f", OLED_data.temperature);
            sh1106_show_string(7,1,str);

            // 清除第6行并显示湿度信息
            sh1106_clear_line(6);  
            sprintf(str, "Humi = %.1f", OLED_data.humidity);
            sh1106_show_string(6,1,str);
        }

        // 刷新显示内容
				sh1106_refresh_gram();
        vTaskDelay(100);                
    }
}
