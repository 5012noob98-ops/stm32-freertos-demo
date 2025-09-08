#include "main.h"
#include "usart1_modbus.h"

/* Modbus �Ĵ��������ּĴ��� Holding Register�� */
uint16_t Modbus_HoldingReg[4] = {

    253,    // 0x0000 �¶� (25.3��)
    605,    // 0x0001 ʪ�� (60.5%)
    0,      // 0x0002 LED ״̬
    0,      // 0x0003 ״̬��

};



uint16_t Modbus_CRC16(uint8_t *buf,uint16_t len){
    uint16_t crc = 0xFFFF;
    for(int pos = 0; pos < len; pos ++){
        crc ^= (uint16_t)buf[pos];  // �������ֽ���CRC�Ĵ�����λ���
        for (int i = 0; i < 8; i++)
        {
           if (crc & 0x0001)// ������λΪ1
           {
            crc >>= 1;  // ����һλ
            crc ^= 0xA001;// �����ʽ���
           }else{
            crc >>= 1;// ����ֻ����һλ
           }      
        }
    }
    return crc;
}


void Modbus_Process(uint8_t *rxBuf,uint16_t len){
    if(len < 8) {
        // HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin); // �л�LED״̬�Թ۲캯��ִ��
        return;// ��С֡��
    }
    
    //У��CRC
    uint16_t crc_calc = Modbus_CRC16(rxBuf,len - 2);
    uint16_t crc_recv = rxBuf[len - 2] | (rxBuf[len - 1] << 8);
    if (crc_calc != crc_recv) {
        HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin); // �л�LED״̬�Թ۲캯��ִ��
        return;
    }

    uint8_t slaveAdrr = rxBuf[0];
    uint8_t funcCode  = rxBuf[1];

    if (slaveAdrr != SLAVE_ID) return;// �Ǳ���ID

    uint8_t txBuf[64];
    uint16_t crc;

    if (funcCode == 0x03) // �����ּĴ���
    {
        uint16_t startAdrr = (rxBuf[2] << 8) | rxBuf[3];
        uint16_t numReg    = (rxBuf[4] << 8) | rxBuf[5];

        txBuf[0] = SLAVE_ID;
        txBuf[1] = 0x03;
        txBuf[2] = numReg * 2;

        for (int i = 0; i < numReg; i++)
        {
            // uint16_t val = Modbus_HoldingReg[startAdrr + i];
            uint16_t val = 0;
            // ʹ�û�������������
            if (xSemaphoreTake(xModbusRegMutex, portMAX_DELAY)) {
                val = Modbus_HoldingReg[startAdrr + i];
                xSemaphoreGive(xModbusRegMutex);
            }


            txBuf[3 + i*2] = val >> 8;
            txBuf[4 + i*2] = val & 0xFF;
        }
        crc = Modbus_CRC16(txBuf,3 + numReg*2);

        txBuf[3 + numReg*2] = crc & 0xFF;
        txBuf[4 + numReg*2] = crc >> 8;

        HAL_UART_Transmit(&huart1, txBuf , 5 + numReg*2,HAL_MAX_DELAY);
    }
    else if (funcCode == 0x06)// д�����Ĵ���
    {
        uint16_t regAddr = (rxBuf[2] << 8) | rxBuf[3];
        uint16_t regVal  = (rxBuf[4] << 8) | rxBuf[5];

        if (regAddr < 4)
        {
            // Modbus_HoldingReg[regAddr] = regVal;
            // ʹ�û�������������
            if (xSemaphoreTake(xModbusRegMutex, portMAX_DELAY)) {
                Modbus_HoldingReg[regAddr] = regVal;
                xSemaphoreGive(xModbusRegMutex);
            }

            // ���⴦�� LED

            if (regAddr == 2)
            {
                if (regVal == 1) HAL_GPIO_WritePin(GPIOC, LED1_Pin, GPIO_PIN_SET);
                else HAL_GPIO_WritePin(GPIOC, LED1_Pin, GPIO_PIN_RESET);
            }  
        }
        
        // ���ԣ���׼ 0x06 ��Ӧ����ԭ�����أ�
        memcpy(txBuf,rxBuf,6);
        crc = Modbus_CRC16(txBuf,6);
        txBuf[6] = crc & 0xFF;
        txBuf[7] = crc >> 8;

        HAL_UART_Transmit(&huart1,txBuf,8,HAL_MAX_DELAY);
    }
}







