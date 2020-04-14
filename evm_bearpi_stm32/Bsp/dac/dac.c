#include "./dac/dac.h"

DAC_HandleTypeDef DAC1_Handler;//DAC���

//��ʼ��DAC
void DAC1_Init(void)
{
    DAC_ChannelConfTypeDef DACCH1_Config;
    
    DAC1_Handler.Instance=DAC;
    HAL_DAC_Init(&DAC1_Handler);                 			//��ʼ��DAC
    
    DACCH1_Config.DAC_Trigger=DAC_TRIGGER_NONE;             //��ʹ�ô�������
    DACCH1_Config.DAC_OutputBuffer=DAC_OUTPUTBUFFER_DISABLE;//DAC1�������ر�
    HAL_DAC_ConfigChannel(&DAC1_Handler,&DACCH1_Config,DAC_CHANNEL_2);//DACͨ��1����
    
    HAL_DAC_Start(&DAC1_Handler,DAC_CHANNEL_2);  			//����DACͨ��1
}

//DAC�ײ�������ʱ�����ã����� ����
//�˺����ᱻHAL_DAC_Init()����
//hdac:DAC���
void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac)
{      
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_DAC1_CLK_ENABLE();             //ʹ��DACʱ��
    __HAL_RCC_GPIOA_CLK_ENABLE();			//����GPIOAʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_4;            //PA4
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;     //ģ��
    GPIO_Initure.Pull=GPIO_NOPULL;          //����������
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
}

void DAC1_DeInit(void)
{
	HAL_DAC_Stop(&DAC1_Handler,DAC_CHANNEL_2);  			//�ر�DACͨ��1
	__HAL_RCC_DAC1_CLK_DISABLE();
}

//����ͨ��1���
void DAC1_Set_Value(uint16_t value)
{
    HAL_DAC_SetValue(&DAC1_Handler,DAC_CHANNEL_2,DAC_ALIGN_12B_R,value);//12λ�Ҷ������ݸ�ʽ����DACֵ
}

