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
//设置IO以及定时器3的输入捕获
void Init_433M(void)
{  
		GPIO_InitTypeDef GPIO_InitStructure,GPIO_InitStructure1;
	
		NVIC_InitTypeDef NVIC_InitStructure;
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		TIM_ICInitTypeDef  TIM_ICInitStructure;  

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); //433发射
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //使能PORTB时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);	 //TIM3 时钟使能 
	
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init(GPIOC,&GPIO_InitStructure);
		GPIO_ResetBits(GPIOC,GPIO_Pin_12);	                     //
		
	 	

		GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_0;				        //PB0 输入
		GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		//浮空输入
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure1);

		TIM_TimeBaseStructure.TIM_Period = 10000;            //设定计数器自动重装值 最大10ms溢出
		TIM_TimeBaseStructure.TIM_Prescaler = (72-1); 	     //预分频器,1M的计数频率,1us加1
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      //设置时钟分割:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式

		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);   					 //根据指定的参数初始化TIMx

		TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;  							// 选择输入端 IC3映射到TI3上
		TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;			//上升沿捕获
		TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
		TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	      		//配置输入分频,不分频
		TIM_ICInitStructure.TIM_ICFilter = 0x03;          							//IC3F=0011 配置输入滤波器      8个定时器时钟周期滤波
		TIM_ICInit(TIM3, &TIM_ICInitStructure);           							//初始化定时器输入捕获通道
		TIM_Cmd(TIM3,ENABLE); 	                                        //使能定时器3
	  
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;     						//TIM3中断
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  			//先占优先级0级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  						//从优先级3级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;     						//IRQ通道被使能
		NVIC_Init(&NVIC_InitStructure);          												//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

		TIM_ITConfig(TIM3,TIM_IT_Update | TIM_IT_CC3,ENABLE);						//允许更新中断 ,允许CC3IE捕获中断
}


/*数据位码*/
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
				temp = Data & 0x80;   //取出最高位
				Data = Data << 1;     //左移 1 位
				if(temp == 0x80)      //最高位为 1
				{
						Data_Bit(1);
				}
				else
				{
						Data_Bit(0);
				}
		}
}

/*起始位*/
void Start_Element(void)
{
		OUT_433M = 1;
		delay_ms(3);           	    // 3ms
		OUT_433M = 0;
		delay_ms(6);			    	    // 6ms
}

/*结束位*/
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
			Start_Element();            //起始位
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












/*数据位码*/
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
				temp = Data & 0x80;   //取出最高位
				Data = Data << 1;     //左移 1 位
				if(temp == 0x80)      //最高位为 1
				{
						Data_Bit1(1);
				}
				else
				{
						Data_Bit1(0);
				}
		}
}

/*起始位*/
void Start_Element1(void)
{
		OUT_433M = 1;
		delay_us(4850);           	    // 3ms
		OUT_433M = 0;
		delay_us(1500);			    	    // 6ms
}

/*结束位*/
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
				Start_Element1();            //起始位
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


