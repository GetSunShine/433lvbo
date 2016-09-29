#include "wuxian433.h"
#include "stm32f10x.h"
#include "delay.h"
#include "stm32f10x_it.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "usart.h"
#include "led.h"
#define TIMENUM    1
//����IO�Լ���ʱ��3�����벶��
void Init_433M(void)
{  
		GPIO_InitTypeDef GPIO_InitStructure,GPIO_InitStructure1;
	
		NVIC_InitTypeDef NVIC_InitStructure;
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		TIM_ICInitTypeDef  TIM_ICInitStructure;  

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); //433����
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //ʹ��PORTBʱ��
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);	 //TIM3 ʱ��ʹ�� 
	
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init(GPIOC,&GPIO_InitStructure);
		GPIO_ResetBits(GPIOC,GPIO_Pin_12);	                     //
		
	 	

		GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_0;				        //PB0 ����
		GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		//��������
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure1);

		TIM_TimeBaseStructure.TIM_Period = 10000;            //�趨�������Զ���װֵ ���10ms���
		TIM_TimeBaseStructure.TIM_Prescaler = (72-1); 	     //Ԥ��Ƶ��,1M�ļ���Ƶ��,1us��1
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      //����ʱ�ӷָ�:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ

		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);   					 //����ָ���Ĳ�����ʼ��TIMx

		TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;  							// ѡ������� IC3ӳ�䵽TI3��
		TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;			//�����ز���
		TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
		TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	      		//���������Ƶ,����Ƶ
		TIM_ICInitStructure.TIM_ICFilter = 0x03;          							//IC3F=0011 ���������˲���      8����ʱ��ʱ�������˲�
		TIM_ICInit(TIM3, &TIM_ICInitStructure);           							//��ʼ����ʱ�����벶��ͨ��
		TIM_Cmd(TIM3,ENABLE); 	                                        //ʹ�ܶ�ʱ��3
	  
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;     						//TIM3�ж�
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  			//��ռ���ȼ�0��
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  						//�����ȼ�3��
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;     						//IRQͨ����ʹ��
		NVIC_Init(&NVIC_InitStructure);          												//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

		TIM_ITConfig(TIM3,TIM_IT_Update | TIM_IT_CC3,ENABLE);						//��������ж� ,����CC3IE�����ж�
}


/*����λ��*/
void Data_Bit(uint8 bit)
{
		if(bit)
		{
			OUT_433M = 1;
			delay_us(1600);          
			OUT_433M = 0;
			delay_us(400);         
		}
		else
		{
			OUT_433M = 1;
			delay_us(400);          
			OUT_433M = 0;
			delay_us(1600);           
	}
}

void Send_Byte(uint8 Data)
{
		uint8 i,temp;
		for(i=0;i<8;i++)
		{
				temp = Data & 0x80;   //ȡ�����λ
				Data = Data << 1;     //���� 1 λ
				if(temp == 0x80)      //���λΪ 1
				{
						Data_Bit(1);
				}
				else
				{
						Data_Bit(0);
				}
		}
}

/*��ʼλ*/
void Start_Element(void)
{
		OUT_433M = 1;
		delay_ms(3);           	    // 3ms
		OUT_433M = 0;
		delay_ms(6);			    	    // 6ms
}

/*����λ*/
void End_Element(void)
{
		Send_Byte(0x0A);
}

void Send_433M(uint8 * Data)
{
	  int i;
		TIM_Cmd(TIM6, DISABLE);     
		TIM_Cmd(TIM4, DISABLE); 
		TIM_Cmd(TIM5, DISABLE); 
		for(i=0;i<5;i++)
		{
			Start_Element();            //��ʼλ
			Send_Byte( *(Data + 0) );   //
			Send_Byte( *(Data + 1) );
			Send_Byte( *(Data + 2) );
			Send_Byte( *(Data + 3) );
			Send_Byte( *(Data + 4) );
			Send_Byte( *(Data + 5) );
			Send_Byte( *(Data + 6) );
			//End_Element();
		}
	  TIM_Cmd(TIM6, ENABLE);    		
}












/*����λ��*/
void Data_Bit1(uint8 bit)
{
		if(bit)
		{
			OUT_433M = 1;
			delay_us(720);          
			OUT_433M = 0;
			delay_us(370);         
		}
		else
		{
			OUT_433M = 1;
			delay_us(370);          
			OUT_433M = 0;
			delay_us(720);           
	}
}

void Send_Byte1(uint8 Data)
{
		uint8 i,temp;
		for(i=0;i<8;i++)
		{
				temp = Data & 0x80;   //ȡ�����λ
				Data = Data << 1;     //���� 1 λ
				if(temp == 0x80)      //���λΪ 1
				{
						Data_Bit1(1);
				}
				else
				{
						Data_Bit1(0);
				}
		}
}

/*��ʼλ*/
void Start_Element1(void)
{
		OUT_433M = 1;
		delay_us(4850);           	    // 3ms
		OUT_433M = 0;
		delay_us(1500);			    	    // 6ms
}

/*����λ*/
void End_Element1(void)
{
		OUT_433M = 1;
		delay_us(3000);           	    // 3ms
		OUT_433M = 0;
		delay_us(3000);			    	    // 6ms
}



void Send_433M1(uint8 * Data)
{
	  int i,j;
		TIM_Cmd(TIM6, DISABLE);     
		TIM_Cmd(TIM4, DISABLE); 
		TIM_Cmd(TIM5, DISABLE); 
//		for(i=0;i<3;i++)
//		{
				Start_Element1();            //��ʼλ
				Send_Byte1( *(Data + 0) );   //
				Send_Byte1( *(Data + 1) );
				Send_Byte1( *(Data + 2) );
				Send_Byte1( *(Data + 3) );
				Send_Byte1( *(Data + 4) );
				//End_Element1();
				//delay_ms(50);	
		//}
	  TIM_Cmd(TIM6, ENABLE);    		
}


