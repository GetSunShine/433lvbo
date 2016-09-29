#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "wuxian433.h"
#include "bluetooth.h"
#include "delay.h"
#include "usart.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "sys.h"
#include "ir.h"
#include "led.h"
#include <string.h>
#include "key.h"

int main(void)
{
		uint8 i,j;
		uint8 key;
		uint8 cmd_433[20]={0};
		uint8 IR_Learn =0;
		uint32 blue_flag;
		uint8 buf[50] ={0},zigbee[50] = {"\0"},dest[100] ={0};
		uint8 test[50] ={0x74,0x41,0x15 ,0xe1,0x55};
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);          //设置NVIC中断分组2:2位抢占优先级，2位响应优先级	
		TIM6_Init();
		delay_init();
		Uart_Init();
		Init_433M();               //433初始化
		BEEP_Init();
		LED_Init();	
		Bluetooth_Init();
		KEY_Init();
		while(1)
		{	
			key = KEY_Scan(0);
			if( Learn_Flag1 == 1 )
			{
				TIM_Cmd(TIM3,DISABLE);
				
				LED = ~LED;				
				
				if( key == KEY0_PRES)
				{	
					Learn_Flag1 = 0;
					LED0 = ~LED0;					
					for( i=0; i<3; i++)
					{
						Send_433M1(Data_433M);
					}
					for( j=0; j<6; j++)
					{
						Send_433M1(Data_433M+5);
					}	
					memset(Data_433M,0,sizeof(Data_433M));
					TIM_Cmd(TIM3,ENABLE);
				}
			}
			
			
		}
}

