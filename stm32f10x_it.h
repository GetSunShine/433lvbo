/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.h 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   This file contains the headers of the interrupt handlers.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F10x_IT_H
#define __STM32F10x_IT_H


#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
	 
#define  USART_BUF_LEN  			50 	    					//定义最大接收字节数 50
#define  USART2_BUF_LEN  			250 	
#define BEEP       PAout(8)      //蜂鸣器输出口PA8
#define LED0 PBout(1)// PB1

/*
#define START_TIME_H         2000
#define START_TIME_MIN_H     1600
#define START_TIME_MAX_H     2400

#define START_TIME_L         7000
#define START_TIME_MIN_L     6600
#define START_TIME_MAX_L     7400	

#define DATA_TIME         2000
#define DATA_TIME_MIN     1500
#define DATA_TIME_MAX     2500
*/



#define START_TIME_H         4850
#define START_TIME_MIN_H    4550
#define START_TIME_MAX_H     5150

#define START_TIME_L         1500
#define START_TIME_MIN_L     1200
#define START_TIME_MAX_L     1800	

#define DATA_TIME         1090
#define DATA_TIME_MIN     840
#define DATA_TIME_MAX     1240


#define END_TIME_H         1000
#define END_TIME_MIN_H     600
#define END_TIME_MAX_H     1400

#define END_TIME_L         4000
#define END_TIME_MIN_L     3600
#define END_TIME_MAX_L     4400	

#define BURRS_TIME_MAX     350

#define DATA_433M_LEN      5
#define DATA_BIT           1
#define END_BYTE           0x41








#define START_TIME_H1         4850
#define START_TIME_MIN_H1    4550
#define START_TIME_MAX_H1     5150

#define START_TIME_L1         1500
#define START_TIME_MIN_L1     1200
#define START_TIME_MAX_L1     1800	

#define DATA_TIME1         1090
#define DATA_TIME_MIN1     890
#define DATA_TIME_MAX1     1290
	 
















extern uint8 Data_433M[20];
extern uint32 Learn_Flag1;


extern int Out_Time;

extern uint8 USART1_RX_BUF[USART_BUF_LEN];
extern uint8 USART2_RX_BUF[USART2_BUF_LEN];
extern uint8 USART3_RX_BUF[USART_BUF_LEN];
extern uint8 USART1_RX_FLAGE;
extern uint8 USART2_RX_FLAGE;
extern uint8 USART3_RX_FLAGE;
extern uint8 RmtSta;	
extern uint8 Status_433M ;	     		  //433的状态
extern uint8 IR_Data[5000];
extern uint8 IR[5000];
extern uint16 H_433[20],L_433[20];     //433的时间	   
extern uint16 i;
extern uint32 Time[2000];
//extern uint32 Learn_Flag, Led_Flag;
extern uint8 num ;
extern uint32 count;

void LED_Init(void);//初始化	
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
void USART3_IRQHandler(void);
void TIM3_IRQHandler(void);
void TIM4_IRQHandler(void);
void TIM6_IRQHandler(void);
void WWDG_IRQHandler(void);
void BEEP_Init(void);
void Buzzer(int ms);
void LED_Light(void);
void CharToHex(char * dest, char * buffer , int len);
#ifdef __cplusplus
}
#endif

#endif /* __STM32F10x_IT_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
