#include "spi_oled.h"

uint8_t s_chDispalyBuffer[128][8]; //8*8bit=64

static void sh1106_reset(void)
{
        //��λ��Ļ
        HAL_GPIO_WritePin(GPIOB, OLED_RES_Pin, GPIO_PIN_RESET);  //RES reset 
        //���߸�λ���ţ�������������ģʽ
        HAL_GPIO_WritePin(GPIOB, OLED_RES_Pin, GPIO_PIN_SET);  //RES set 
}

//����ָ��
static void sh1106_write_cmd(uint8_t chData) 
{
        HAL_GPIO_WritePin(GPIOB, OLED_DC_Pin, GPIO_PIN_RESET);//����DC������ָ��  
        HAL_SPI_Transmit(&hspi3, &chData, 1, 0xff);//����
}       

//��������
static void sh1106_write_data(uint8_t chData) 
{
        HAL_GPIO_WritePin(GPIOB, OLED_DC_Pin, GPIO_PIN_SET);  //����DC����������  
        HAL_SPI_Transmit(&hspi3, &chData, 1, 0xff);//����
} 

void sh1106_init(void)
{       
        sh1106_reset();
        sh1106_write_cmd(0xAE);//--turn off oled panel
        sh1106_write_cmd(0x00);//---set low column address 00->02
        sh1106_write_cmd(0x10);//---set high column address
        sh1106_write_cmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
        sh1106_write_cmd(0x81);//--set contrast control register
        sh1106_write_cmd(0xCF);// Set SEG Output Current Brightness
        sh1106_write_cmd(0xA1);//--Set SEG/Column Mapping     
        sh1106_write_cmd(0xC0);//Set COM/Row Scan Direction   
        sh1106_write_cmd(0xA6);//--set normal display
        sh1106_write_cmd(0xA8);//--set multiplex ratio(1 to 64)
        sh1106_write_cmd(0x3f);//--1/64 duty
        sh1106_write_cmd(0xD3);//-set display offset Shift Mapping RAM Counter (0x00~0x3F)
        sh1106_write_cmd(0x00);//-not offset
        sh1106_write_cmd(0xd5);//--set display clock divide ratio/oscillator frequency
        sh1106_write_cmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
        sh1106_write_cmd(0xD9);//--set pre-charge period
        sh1106_write_cmd(0xF1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
        sh1106_write_cmd(0xDA);//--set com pins hardware configuration
        sh1106_write_cmd(0x12);
        sh1106_write_cmd(0xDB);//--set vcomh
        sh1106_write_cmd(0x40);//Set VCOM Deselect Level
        sh1106_write_cmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
        sh1106_write_cmd(0x02);//
        sh1106_write_cmd(0x8D);//--set Charge Pump enable/disable
        sh1106_write_cmd(0x14);//--set(0x10) disable
        sh1106_write_cmd(0xA4);// Disable Entire Display On (0xa4/0xa5)
        sh1106_write_cmd(0xA6);// Disable Inverse Display On (0xa6/a7) 
        sh1106_write_cmd(0xAF);//--turn on oled panel
}


void sh1106_clear_line(uint8_t page) {
    if (page >= 8) return;
    for (uint8_t x = 0; x < 128; x++) {
        s_chDispalyBuffer[x][page] = 0x00;
    }
}


void sh1106_clear_screen(void)  
{ 
        uint8_t i, j;
        for (i = 0; i < 8; i ++) {
                sh1106_write_cmd(0xB0 + i); //����ҳ���0xB0��ʼ��0xB7
                sh1106_write_cmd(0x02); //����ʼ��ַ����λ
                sh1106_write_cmd(0x10); //����ʼ��ַ����λ
                //8*128���㣬ȫ������
                for (j = 0; j < 128; j ++) {
                    s_chDispalyBuffer[j][i] = 0; //���0
                    sh1106_write_data(s_chDispalyBuffer[j][i]); //��������
                }
        }
}

/**
  ����Ҫ�����ĵ�ת��Ϊ��ʾ����s_chDispalyBuffer��һ��bit״̬
  chXpos: ���Ƶ��x���� 0<= x <=127
  chYpos: ���Ƶ��y���� 0<= 7 <=63
  chPoint: 0: Ϩ��  1: ���� 
**/
void sh1106_draw_point(uint8_t chXpos, uint8_t chYpos, uint8_t chPoint)
{
        uint8_t chPos, chBx, chTemp = 0;
        
        if (chXpos > 127 || chYpos > 63) {
                return;
        }
        //chYpos����ת������Ϊ������8���ֽڹ�����64��bit,������Ҫ��y����ת������Ӧ���ֽ�bitλ��
        chPos = 7 - chYpos / 8;   //�ҳ���һҳ
        chBx = chYpos % 8;        //�ҳ���һλ
        chTemp = 1 << (7 - chBx); //�Ѷ�Ӧλ��1
        if (chPoint) {
            s_chDispalyBuffer[chXpos][chPos] |= chTemp;
        } else {
            s_chDispalyBuffer[chXpos][chPos] &= ~chTemp;
        }
}

/**
����ҳ���µ���Ļ��ʾ
����ʾ����s_chDispalyBuffer���͵���Ļ��ʾ
**/
void sh1106_refresh_gram(void)
{
        uint8_t i, j;
        for (i = 0; i < 8; i ++) {  
                sh1106_write_cmd(0xB0 + i); //����ҳ���0xB0��ʼ��0xB7   
                sh1106_write_cmd(0x02); //����ʼ��ַ����λ
                sh1106_write_cmd(0x10); //����ʼ��ַ����λ      
                for (j = 0; j < 128; j ++) {
                        sh1106_write_data(s_chDispalyBuffer[j][i]); 
                }
        }   
}

void sh1106_draw_bitmap(uint8_t chXpos, uint8_t chYpos, const uint8_t *pchBmp, uint8_t chWidth, uint8_t chHeight)
{
    uint16_t i, j, byteWidth = (chWidth + 7) / 8;
    //����ͼƬ�Ŀ�ߣ�ȡ��ÿһ�㣬�ж�Ϊ1��λ��Ϊ��Ҫ�����ĵ㣬ͨ�����㺯�����Ƶ���Ļ
    for(j = 0; j < chHeight; j ++){
        for(i = 0; i < chWidth; i ++ ) {
            if(*(pchBmp + j * byteWidth + i / 8) & (128 >> (i & 7))) {
                sh1106_draw_point(chXpos + i, chYpos + j, 1);
            }
        }
    }
}

/**
  * @brief  OLED���ù��λ��
  * @param  Y �����Ͻ�Ϊԭ�㣬���·�������꣬��Χ��0~7
  * @param  X �����Ͻ�Ϊԭ�㣬���ҷ�������꣬��Χ��0~127
  * @retval ��
  */
void sh1106_set_cursor(uint8_t Y, uint8_t X)
{
	sh1106_write_cmd(0xB0 | Y); //����Yλ��
	sh1106_write_cmd(0x10 | ((X & 0xF0) >> 4));  //����Xλ�ø�4λ
	sh1106_write_cmd(0x00 | (X & 0x0F)); //����Xλ�õ�4λ			
}


/**
  * @brief  OLED�����ַ���ʾ
  * @param  page 		�кţ������Ͻ�Ϊԭ�㣬���·�������꣬��Χ��0~7
  * @param  column 	�кţ������Ͻ�Ϊԭ�㣬���ҷ�������꣬��Χ��0~15
  * @param  Char 		��ʾ�ַ�
  * @retval ��
  */
void sh1106_show_char(uint8_t page, uint8_t column, char Char) {
    uint8_t i;
    uint8_t c = Char - ' ';
    if (c >= 95) return; // Խ�籣��

    for (i = 0; i < CHARACTER_W; i++) {
        s_chDispalyBuffer[column * (CHARACTER_W+1) + i][page] = Characters[c][i];
    }

    // ��ѡ����һ���������ּ��
    s_chDispalyBuffer[column * (CHARACTER_W+1) + CHARACTER_W][page] = 0x00;
}


/**
  * @brief  OLED�����ַ�����ʾ
  * @param  Line 		�кţ������Ͻ�Ϊԭ�㣬���·�������꣬��Χ��0~7
  * @param  Column 	�кţ������Ͻ�Ϊԭ�㣬���ҷ�������꣬��Χ��0~15
  * @param  String 	��ʾ�ַ���
  * @retval ��
  */
void sh1106_show_string(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		sh1106_show_char(Line, Column + i, String[i]);
	}
    // sh1106_refresh_gram();
}

