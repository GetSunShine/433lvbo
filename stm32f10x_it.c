#include "stm32f10x_it.h"
#include "stdio.h"
#include "string.h"
#include "usart.h"
#include "led.h"
#include "sys.h"
#include "ir.h"
#include "wuxian433.h"
#include "delay.h"

uint8 USART1_RX_BUF[USART_BUF_LEN];
uint8 USART2_RX_BUF[USART2_BUF_LEN];
uint8 USART3_RX_BUF[USART_BUF_LEN];
uint8 USART1_RX_FLAGE=0;      			  //接收状态标记
uint8 USART2_RX_FLAGE=0;      			  //接收状态标记
uint8 USART3_RX_FLAGE=0;     				  //接收状态标记
static uint8 usart1_num=0;
static uint8 usart2_num=0;
static uint8 usart3_num=0;

int Out_Time = 0;

void USART1_IRQHandler(void)
{
		uint8 temp;
		if( USART_GetITStatus(USART1,USART_IT_RXNE) != RESET )
		{
				temp=USART_ReceiveData(USART1);
				USART1_RX_BUF[usart1_num]=temp;	
				if( (USART1_RX_BUF[usart1_num] == 0x0A) && (USART1_RX_BUF[usart1_num - 1] == 0x0D) )
				{			
						USART1_RX_FLAGE = 1;
						usart1_num = 0;
				}
				else
				{
						usart1_num ++;
				}
	  }
}

void USART2_IRQHandler(void)
{
		uint8 temp;
		if( USART_GetITStatus(USART2,USART_IT_RXNE) != RESET )
		{
				temp=USART_ReceiveData(USART2);
				USART2_RX_BUF[usart2_num]=temp;	
				if( (USART2_RX_BUF[usart2_num] == 0x0A) && (USART2_RX_BUF[usart2_num - 1] == 0x0D) )
				{			
						USART2_RX_FLAGE = 1;
						usart2_num = 0;
				}
				else
				{
						usart2_num ++;
				}
	  }
}

void USART3_IRQHandler(void)
{
		uint8 temp;
		if( USART_GetITStatus(USART3,USART_IT_RXNE) != RESET )
		{
				temp=USART_ReceiveData(USART3);
				USART3_RX_BUF[usart3_num]=temp;	
				if( (USART3_RX_BUF[usart3_num] == 0x0A) && (USART3_RX_BUF[usart3_num - 1] == 0x0D) )
				{			
						USART3_RX_FLAGE = 1;
						usart3_num = 0;
				}
				else
				{
						usart3_num ++;
				}
	  }
}

//定时器5中断服务程序
void TIM5_IRQHandler(void)          //TIM5中断
{
		if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)  //检查TIM5更新中断发生与否
		{
				TIM_ClearITPendingBit(TIM5, TIM_IT_Update);     //清除TIMx更新中断标志
		  	IR_LED = !IR_LED;
		}
}

//定时器6中断服务程序
void TIM6_IRQHandler(void)                             //TIM6中断
{
		if(TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)  //检查TIM6更新中断发生与否
		{
				TIM_ClearITPendingBit(TIM6, TIM_IT_Update);    //清除TIMx更新中断标志
				Out_Time ++;
		}
}


uint8  RmtSta = 0;
static uint16 Dval;		      		//下降沿时计数器的值
uint8  DATA[50] = {"\0"};
uint8  IR_Data[5000] ={0};
uint16 i = 0,j;
uint32 Time[2000];
uint8 num = 0;
uint32 count = 0;
uint8 IR[5000] ={0};
void TIM4_IRQHandler(void)
{
		if(TIM_GetITStatus(TIM4,TIM_IT_Update) != RESET)
		{
					num ++;
					count ++;
					if(num == 0x18)
					{
							if(Time[0]!=0)
							{			
									RmtSta =0;
									count = 0;
									num = 0;
									strcat(IR_Data,"IR:1,38000,");
									for(j =0; j<i;j++)
									{
											sprintf(DATA,"%d,",((Time[j]*38)/1000));
											strcat(IR_Data,DATA);
											memset(DATA,0,sizeof(DATA));
									}
									strcat(IR_Data,"\r\n");
									USART2_Send(IR_Data);
									if(strlen(IR_Data) > 200)
									{Buzzer(100);}
									memset(IR_Data,0,sizeof(IR_Data));
									Time[0] = 0; 	
									i = 0;
									TIM_Cmd(TIM4,DISABLE );
							}
					}
		}		
		if(TIM_GetITStatus(TIM4,TIM_IT_CC4) != RESET)
		{
				if(RDATA)																								//捕获上升沿
				{
						Dval = TIM_GetCapture4(TIM4);		
						TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Falling); //设置为下降沿捕获			
						TIM_SetCounter(TIM4,0);						                  //清空定时器的值	
						if( i > 1000)
						{
								i = 0;
						}
						Time[i] = Dval;
						i++;							
						RmtSta |= 0x10;		
						count = 0;
				}
				else   																									//捕获下降沿  
				{
						Dval = TIM_GetCapture4(TIM4);					          			           
						TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Rising);	//设置为上升沿捕获
						TIM_SetCounter(TIM4,0);	
						if(RmtSta & 0x10)							           
						{		
									if( i >1000)
									{
											i = 0;
									}
									Time[i] = (count*10000)+Dval;			//当低电平的时间溢出定时器的时候,定时器10ms溢出,当接收达到高电平的时候把这个计数清零
									i++;
									num =0;							
					  }
				}					
		}
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update | TIM_IT_CC4);
}


/*

static uint16 time_H = 0;
static uint16 time_L = 0;
static uint8 start_flag_H = 0;
static uint8 start_flag_L = 0;
static uint16 start_time_H = 0;
static uint16 start_time_L = 0;

static uint8 data_flag_H = 0;
static uint8 data_flag_L = 0;
static uint16 data_time_H = 0;
static uint16 data_time_L = 0;
static short Deviation_Time = 0;

uint8 Status_433M = 0;
static uint8 Rec_433M = 0;	   //433M接收到的数据
uint8 Data_433M[20] = {0};
static uint8 count_433M = 0;
static uint8 Data_433M_Len = 0;
static uint8 ret;
uint32 Learn_Flag, Led_Flag;
uint8  Data[50] = {0},send_433M[50] = {0};


void TIM3_IRQHandler(void)
{
		if(TIM_GetITStatus(TIM3,TIM_IT_Update) == SET)
		{
				
		}
		if(TIM_GetITStatus(TIM3,TIM_IT_CC3) == SET)
		{
				if(MDATA)  									        			//上升沿捕获
		 		{
						time_L = TIM_GetCapture3(TIM3);                         //捕获低电平的时间
						TIM_SetCounter(TIM3,0);																				//清空定时器值
						TIM_OC3PolarityConfig(TIM3,TIM_ICPolarity_Falling);						//CC3P=1设置为下降沿捕获	
						if( !(Status_433M & 0x10)	)                                   //捕获引导码高电平
						{
								if( start_flag_H == 1 )
								{
										if( time_L < BURRS_TIME_MAX )
										{
												start_time_H += time_L;
										}
										else
										{
												Status_433M = 0;
												start_time_H = 0;
												start_flag_H = 0;
												Deviation_Time = 0;
										}
								}
						}
						else 
						{
								if( !(Status_433M & 0x80)	)  
								{		
										if( ( time_L >= (START_TIME_MIN_L - Deviation_Time) ) && ( time_L <= (START_TIME_MAX_L - Deviation_Time) ) )
										{
												Deviation_Time = time_L - START_TIME_L;
												Status_433M |= 0x80;
												//TIM_Cmd(TIM6, DISABLE); 
												//TIM_Cmd(TIM4, DISABLE);
												Status_433M &= ~(1<<5);
												data_time_L = 0;
												data_flag_L = 0;
												data_time_H = 0;
												data_flag_H = 0;
										}
										else if( (time_L > START_TIME_MAX_L) )
										{
												Status_433M = 0;
												start_flag_H = 0;
												start_flag_L = 0;
												start_time_H = 0;
												start_time_L = 0;
												Deviation_Time = 0;
										}
										else
										{
												Status_433M &= ~(1 << 7);
												start_flag_L = 1;
												start_time_L += time_L;
												if(( start_time_L >= (START_TIME_MIN_L - Deviation_Time) ) && ( start_time_L <= (START_TIME_MAX_L - Deviation_Time)) )
												{	
														Deviation_Time = start_time_L - START_TIME_L;
														Status_433M |= 0x80;
														//TIM_Cmd(TIM6, DISABLE); 
														//TIM_Cmd(TIM4, DISABLE);  													
														Status_433M &= ~(1<<5);		
														data_time_L = 0;
														data_flag_L = 0;
														data_time_H = 0;
														data_flag_H = 0;
												}
												else
												{
														if( start_time_L > START_TIME_MAX_L )
														{
																Status_433M = 0;
																start_flag_H = 0;
																start_flag_L = 0;
																start_time_H = 0;
																start_time_L = 0;
																Deviation_Time = 0;
														}
														else
														{
																Status_433M &= ~(1 << 7);
																start_flag_L = 1;
														}
												}
										}
								}
								else
								{
										if( !(Status_433M & 0x20)	)                                
										{
												
												data_time_L += time_L;
												if(( (data_time_L + data_time_H) >= (DATA_TIME_MIN - Deviation_Time) ) && ( (data_time_L + data_time_H) <= (DATA_TIME_MAX - Deviation_Time)) )
												{
														Deviation_Time = (data_time_L + data_time_H) - DATA_TIME;
													
														Rec_433M <<= 1;								//左移一位.
														if(data_time_H > data_time_L)
														{
																Rec_433M |= 1;								//接收到1
														}
														else
														{
																Rec_433M &= ~1;								//接收到0
														}
														count_433M ++;
														if(count_433M > 7)
														{
																count_433M = 0;
																Data_433M[Data_433M_Len] = Rec_433M;
																Data_433M_Len ++;
																if(Data_433M_Len == 7)
																{
																		Status_433M |= 0x20;
																		Status_433M &= ~(1<<3);		
																}
																else
																{
																		Status_433M &= ~(1<<5);			
																}
														}
														data_time_L = 0;
														data_time_H = 0;
												}
												else if( (time_L > DATA_TIME_MAX) && (data_time_L > DATA_TIME_MAX) )
												{
														Status_433M = 0;
														Deviation_Time = 0;
														start_flag_H = 0;
														start_time_H = 0;
														count_433M = 0;
														Data_433M_Len = 0;
														memset(Data_433M,0,sizeof(Data_433M));
												}
										}
										else
										{
												if( !(Status_433M & 0x8))        
												{ 
														//TIM_Cmd(TIM6, ENABLE); 
														//if(((Data_433M[0] == 0x53) || (Data_433M[0] == 0x58))&& (Data_433M_Len == 7))
														if((Data_433M[0] == 0x53)&& (Data_433M_Len == 7))
														{
																	for(i =0;i<7;i++)	
																	{
																				sprintf(Data+2*i,"%.2x",Data_433M[i]);             //数据转换成16进制
																	}
																	ret=strncmp((Data+2),"4c",2);
																	if((Learn_Flag==1) && (ret==0))    //如果需要数据且是学习码 
																	{	 
																			strcpy(send_433M,"RF433:");
																			strcat(send_433M,Data);
																			strcat(send_433M,"\r\n");
																		  USART2_Send(send_433M);
																			Buzzer(200);
																	}
																	else if (ret != 0)
																	{
																			strcpy(send_433M,"RF433:");
																			strcat(send_433M,Data);
																			strcat(send_433M,"\r\n");
																		  USART2_Send(send_433M);
																	} 
																	LED_Light();
																	memset(Data,0,sizeof(Data)); 
																	memset(USART2_RX_BUF,0,sizeof(USART2_RX_BUF));														
																	memset(send_433M,0,sizeof(send_433M));     //清空串口发出的buf
																	memset(Data_433M,0,sizeof(Data_433M));  
														}				
														Status_433M = 0;		
														start_time_H = 0;
														start_time_L = 0;
														start_flag_H = 0;
														start_flag_L = 0;
														data_time_H = 0;
														data_time_L = 0;
														data_flag_H = 0;
														data_flag_L = 0;
														Deviation_Time = 0;
														count_433M = 0;
														Data_433M_Len = 0;
														memset(Data_433M,0,sizeof(Data_433M));
											 }
										}
								}
						}
				}
				else     																			 //下降沿捕获
				{
						time_H = TIM_GetCapture3(TIM3);	         	 //捕获高电平的时间
						TIM_SetCounter(TIM3,0);						                          	//清空定时器值	
						TIM_OC3PolarityConfig(TIM3,TIM_ICPolarity_Rising);						//CC3P=1设置为下降沿捕获		
						if( !(Status_433M & 0x10)	)                                   //捕获引导码高电平
						{
								if( (time_H >= START_TIME_MIN_H) && (time_H <= START_TIME_MAX_H) )  
								{
									
										Deviation_Time = time_H - START_TIME_H;
										Status_433M |= 0x10;							 //标记上升沿已经被捕
										Status_433M &= ~(1<<7);
										start_flag_H = 0;
										start_flag_L = 0;
										start_time_H = 0;
										start_time_L = 0;
								}
								else if( (time_H > START_TIME_MAX_H) )
								{
										Status_433M = 0;
										start_flag_H = 0;
										start_time_H = 0;
										Deviation_Time = 0;
								}
								else
								{
										Status_433M &= ~(1<<4);
										start_flag_H = 1;
										start_time_H += time_H;
										if(start_time_H != time_H)
										{
												if( (start_time_H != time_H) && (start_time_H >= START_TIME_MIN_H) && (start_time_H <= START_TIME_MAX_H) )  	 
												{	
														Deviation_Time = start_time_H - START_TIME_H;
														Status_433M |= 0x10;											 //标记上升沿已经被捕
														Status_433M &= ~(1<<7);
														start_flag_H = 0;
														start_flag_L = 0;
														start_time_H = 0;
														start_time_L = 0;
												}
												else
												{
														if( start_time_H > START_TIME_MAX_H )
														{
																Status_433M = 0;
																start_flag_H = 0;
																start_time_H = 0;
																Deviation_Time = 0;
														}
														else
														{
																Status_433M &= ~(1<<4);
																start_flag_H = 1;
														}
												}
										}
								}
						}
						else  																								//捕获引导码低电平
						{
								if( !(Status_433M & 0x80)	)     
								{
										if( start_flag_L == 1 )      	
										{
												if( time_H < 450 )
												{
														start_time_L += time_H;
												}
												else
												{
														Status_433M = 0;
														start_flag_H = 0;
														start_flag_L = 0;
														start_time_H = 0;
														start_time_L = 0;
														Deviation_Time = 0;
												}
										}
								}
								else
								{
										if( !(Status_433M & 0x20)	)
										{
												data_time_H += time_H;
												if( (Status_433M & 0x40) && ( (data_time_L + data_time_H) >= (DATA_TIME_MIN - Deviation_Time) ) && ( (data_time_L + data_time_H) <= (DATA_TIME_MAX - Deviation_Time)) )
												{
														Deviation_Time = (data_time_L + data_time_H) - DATA_TIME;
														
														Rec_433M <<= 1;								//左移一位.
														if(data_time_H > data_time_L)
														{
																Rec_433M |= 1;								//接收到1
														}
														else
														{
																Rec_433M &= ~1;								//接收到0
														}
														count_433M ++;
														if(count_433M > 7)
														{
																count_433M = 0;
																Data_433M[Data_433M_Len] = Rec_433M;
																Data_433M_Len ++;
																if(Data_433M_Len == 7)
																{
																		Status_433M |= 0x20;
																		Status_433M &= ~(1<<3);		
																}
																else
																{
																		Status_433M &= ~(1<<5);			
																}
														}
														data_time_L = 0;
														data_time_H = 0;
												}
												else if( (time_L > DATA_TIME_MAX) && (data_time_L > DATA_TIME_MAX) )
												{
														Status_433M = 0;
														Deviation_Time = 0;
														start_flag_H = 0;
														start_time_H = 0;
														count_433M = 0;
														Data_433M_Len = 0;
														memset(Data_433M,0,sizeof(Data_433M));
												}
										}
										else
										{
												if( !(Status_433M & 0x8)	)        
												{ 
														//TIM_Cmd(TIM6, ENABLE);  
														//if(((Data_433M[0] == 0x53) || (Data_433M[0] == 0x58))&& (Data_433M_Len == 7))
														if((Data_433M[0] == 0x53)&& (Data_433M_Len == 7))													
														{
																	for(i =0;i<7;i++)	
																	{
																				sprintf(Data+2*i,"%.2x",Data_433M[i]);             //数据转换成16进制
																	}
																	ret=strncmp((Data+2),"4c",2);
																	if((Learn_Flag==1) && (ret==0))    //如果需要数据且是学习码 
																	{	 
																			strcpy(send_433M,"RF433:");
																			strcat(send_433M,Data);
																			strcat(send_433M,"\r\n");
																			USART2_Send(send_433M);
																			Buzzer(200);
																	}
																	else if (ret != 0)
																	{
																			strcpy(send_433M,"RF433:");
																			strcat(send_433M,Data);
																			strcat(send_433M,"\r\n");
																	   	USART2_Send(send_433M);
																	} 
																	LED_Light();
																	memset(Data,0,sizeof(Data)); 
																	memset(USART2_RX_BUF,0,sizeof(USART2_RX_BUF));														
																	memset(send_433M,0,sizeof(send_433M));     //清空串口发出的buf
																	memset(Data_433M,0,sizeof(Data_433M));  
														}
														Status_433M = 0;		
														start_time_H = 0;
														start_time_L = 0;
														start_flag_H = 0;
														start_flag_L = 0;
														data_time_H = 0;
														data_time_L = 0;
														data_flag_H = 0;
														data_flag_L = 0;
														Deviation_Time = 0;
														count_433M = 0;
														Data_433M_Len = 0;
														memset(Data_433M,0,sizeof(Data_433M));	
											
												}						
										}
								}
						}	
				}
		}
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update | TIM_IT_CC3);	 	  
}

*/










static uint16 time_H = 0;
static uint16 time_L = 0;
static uint8 start_flag_H = 0;
static uint8 start_flag_L = 0;
static uint16 start_time_H = 0;
static uint16 start_time_L = 0;

static uint8 data_flag_H = 0;
static uint8 data_flag_L = 0;
static uint16 data_time_H = 0;
static uint16 data_time_L = 0;
static short Deviation_Time = 0;

uint8 Status_433M = 0;
static uint8 Rec_433M = 0;	   //433M接收到的数据
uint8 Data_433M[20] = {0};
uint8 Temp_Data_433M[20] = {0};
static uint8 count_433M = 0;
static uint8 Data_433M_Len = 0;
static uint8 ret;
uint32 Learn_Flag1=0, Led_Flag;
uint8  Data[50] = {0},send_433M[50] = {0};


void TIM3_IRQHandler(void)
{
		if(TIM_GetITStatus(TIM3,TIM_IT_Update) == SET)
		{
				
		}
		if(TIM_GetITStatus(TIM3,TIM_IT_CC3) == SET)
		{
				if(MDATA)  									        			//上升沿捕获
		 		{
						time_L = TIM_GetCapture3(TIM3);                         //捕获低电平的时间
						TIM_SetCounter(TIM3,0);																				//清空定时器值
						TIM_OC3PolarityConfig(TIM3,TIM_ICPolarity_Falling);						//CC3P=1设置为下降沿捕获	
						if( !(Status_433M & 0x10)	)                                   //捕获引导码高电平
						{
								if( start_flag_H == 1 )
								{
										if( time_L < BURRS_TIME_MAX )
										{
												start_time_H += time_L;
										}
										else
										{
												Status_433M = 0;
												start_time_H = 0;
												start_flag_H = 0;
												Deviation_Time = 0;
										}
								}
						}
						else 
						{
								if( !(Status_433M & 0x80)	)  
								{		
										if( ( time_L >= (START_TIME_MIN_L - Deviation_Time) ) && ( time_L <= (START_TIME_MAX_L - Deviation_Time) ) )
										{
												Deviation_Time = time_L - START_TIME_L;
												Status_433M |= 0x80;
												Status_433M &= ~(1<<5);
												data_time_L = 0;
												data_flag_L = 0;
												data_time_H = 0;
												data_flag_H = 0;
										}
										else if( (time_L > START_TIME_MAX_L) )
										{
												Status_433M = 0;
												start_flag_H = 0;
												start_flag_L = 0;
												start_time_H = 0;
												start_time_L = 0;
												Deviation_Time = 0;
										}
										else
										{
												Status_433M &= ~(1 << 7);
												start_flag_L = 1;
												start_time_L += time_L;
												if(( start_time_L >= (START_TIME_MIN_L - Deviation_Time) ) && ( start_time_L <= (START_TIME_MAX_L - Deviation_Time)) )
												{	
														Deviation_Time = start_time_L - START_TIME_L;
														Status_433M |= 0x80;
														//TIM_Cmd(TIM6, DISABLE); 
														//TIM_Cmd(TIM4, DISABLE);  													
														Status_433M &= ~(1<<5);		
														data_time_L = 0;
														data_flag_L = 0;
														data_time_H = 0;
														data_flag_H = 0;
												}
												else
												{
														if( start_time_L > START_TIME_MAX_L )
														{
																Status_433M = 0;
																start_flag_H = 0;
																start_flag_L = 0;
																start_time_H = 0;
																start_time_L = 0;
																Deviation_Time = 0;
														}
														else
														{
																Status_433M &= ~(1 << 7);
																start_flag_L = 1;
														}
												}
										}
								}
								else
								{
										if( !(Status_433M & 0x20)	)                                
										{
												data_time_L += time_L;
												if( 
													( ( (data_time_L + data_time_H) >= (DATA_TIME_MIN - Deviation_Time) ) && \
													( (data_time_L + data_time_H) <= (DATA_TIME_MAX - Deviation_Time) ) ) || \
													( ( count_433M == 39) && ( (data_time_L + data_time_H) >= (DATA_TIME_MIN - Deviation_Time) ) && \
													( (data_time_L + data_time_H) <= (DATA_TIME_MAX - Deviation_Time - 370 + 8800 ) ) )
												)
												{
														Deviation_Time = (data_time_L + data_time_H) - DATA_TIME;
													
														Rec_433M <<= 1;								//左移一位.
														if(data_time_H > data_time_L)
														{
																Rec_433M |= 1;								//接收到1
														}
														else
														{
																Rec_433M &= ~1;								//接收到0
														}
														count_433M ++;
														if( !(count_433M % 8) )
														{
																Data_433M[Data_433M_Len] = Rec_433M;
																Data_433M_Len ++;
																if(Data_433M_Len == DATA_433M_LEN)
																{
																		Status_433M |= 0x20;
																		Status_433M &= ~(1<<3);																			
																}
																else
																{
																		Status_433M &= ~(1<<5);			
																}
														}
														data_time_L = 0;
														data_time_H = 0;
												}
												else if( ((time_L > DATA_TIME_MAX) && (data_time_L > DATA_TIME_MAX))/* || ((time_L > 60000) && (data_time_L > 60000))*/ )
												{
														Status_433M = 0;
														Deviation_Time = 0;
														start_flag_H = 0;
														start_time_H = 0;
														count_433M = 0;
														Data_433M_Len = 0;
														memset(Data_433M,0,sizeof(Data_433M));
												}
										}
										else
										{
												if( !(Status_433M & 0x8))        
												{ 
																		Data_433M[4] |= 1;			
																		if( Data_433M[4] == 0x55 )
																		{
																		
																		}
																		else
																		{
																				Data_433M[5] = Data_433M[0];
																				Data_433M[6] = Data_433M[1];
																				Data_433M[7] = Data_433M[2];
																				Data_433M[8] = Data_433M[3];
																	//			Data_433M[9] = (0x0f - (Data_433M[4] & 0x0f)) | (Data_433M[4] & 0xf0);
																				Data_433M[9] = ( ~Data_433M[4] & 0x0f ) |(Data_433M[4] & 0xf0);
																		}
																				
																Learn_Flag1 = 1;
																LED = !LED;
																USART_SendData(USART1,Data_433M[0]);
																while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
															
																	USART_SendData(USART1,Data_433M[1]);
																while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
															
																	USART_SendData(USART1,Data_433M[2]);
																while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
																	USART_SendData(USART1,Data_433M[3]);
																while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
															
																	USART_SendData(USART1,Data_433M[4]);
																while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
													
																	USART_SendData(USART1,Data_433M[5]);
																while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
															
																	USART_SendData(USART1,Data_433M[6]);
																while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
															
																	USART_SendData(USART1,Data_433M[7]);
																while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
																	USART_SendData(USART1,Data_433M[8]);
																while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
															
																	USART_SendData(USART1,Data_433M[9]);
																while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
															
														Status_433M = 0;		
														start_time_H = 0;
														start_time_L = 0;
														start_flag_H = 0;
														start_flag_L = 0;
														data_time_H = 0;
														data_time_L = 0;
														data_flag_H = 0;
														data_flag_L = 0;
														Deviation_Time = 0;
														count_433M = 0;
														Data_433M_Len = 0;
														//memset(Data_433M,0,sizeof(Data_433M));
														delay_ms(500);
											 }
										}
								}
						}
				}
				else     																			 //下降沿捕获
				{
						time_H = TIM_GetCapture3(TIM3);	         	 //捕获高电平的时间
						TIM_SetCounter(TIM3,0);						                          	//清空定时器值	
						TIM_OC3PolarityConfig(TIM3,TIM_ICPolarity_Rising);						//CC3P=1设置为下降沿捕获		
						if( !(Status_433M & 0x10)	)                                   //捕获引导码高电平
						{
								if( (time_H >= START_TIME_MIN_H) && (time_H <= START_TIME_MAX_H) )  
								{
									
										Deviation_Time = time_H - START_TIME_H;
										Status_433M |= 0x10;							 //标记上升沿已经被捕
										Status_433M &= ~(1<<7);
										start_flag_H = 0;
										start_flag_L = 0;
										start_time_H = 0;
										start_time_L = 0;
								}
								else if( (time_H > START_TIME_MAX_H) )
								{
										Status_433M = 0;
										start_flag_H = 0;
										start_time_H = 0;
										Deviation_Time = 0;
								}
								else
								{
										Status_433M &= ~(1<<4);
										start_flag_H = 1;
										start_time_H += time_H;
										if(start_time_H != time_H)
										{
												if( (start_time_H != time_H) && (start_time_H >= START_TIME_MIN_H) && (start_time_H <= START_TIME_MAX_H) )  	 
												{	
														Deviation_Time = start_time_H - START_TIME_H;
														Status_433M |= 0x10;											 //标记上升沿已经被捕
														Status_433M &= ~(1 << 7);
														start_flag_H = 0;
														start_flag_L = 0;
														start_time_H = 0;
														start_time_L = 0;
												}
												else
												{
														if( start_time_H > START_TIME_MAX_H )
														{
																Status_433M = 0;
																start_flag_H = 0;
																start_time_H = 0;
																Deviation_Time = 0;
														}
														else
														{
																Status_433M &= ~(1<<4);
																start_flag_H = 1;
														}
												}
										}
								}
						}
						else  																								//捕获引导码低电平
						{
								if( !(Status_433M & 0x80)	)     
								{
										if( start_flag_L == 1 )      	
										{
												if( time_H < BURRS_TIME_MAX )
												{
														start_time_L += time_H;
												}
												else
												{
														Status_433M = 0;
														start_flag_H = 0;
														start_flag_L = 0;
														start_time_H = 0;
														start_time_L = 0;
														Deviation_Time = 0;
												}
										}
								}
								else
								{
										if( !(Status_433M & 0x20)	)
										{
												data_time_H += time_H;
												if( 
													( (Status_433M & 0x40) && ( (data_time_L + data_time_H) >= (DATA_TIME_MIN - Deviation_Time) ) && \
													( (data_time_L + data_time_H) <= (DATA_TIME_MAX - Deviation_Time) ) ) || \
													( ( count_433M == 39) && ( (data_time_L + data_time_H) >= (DATA_TIME_MIN - Deviation_Time) ) && \
													( (data_time_L + data_time_H) <= (DATA_TIME_MAX - Deviation_Time - 370 + 8800 ) ) )
												)
												{
														Deviation_Time = (data_time_L + data_time_H) - DATA_TIME;
														
														Rec_433M <<= 1;								//左移一位.
														if(data_time_H > data_time_L)
														{
																Rec_433M |= 1;								//接收到1
														}
														else
														{
																Rec_433M &= ~1;								//接收到0
														}
														count_433M ++;
														if( !(count_433M % 8) )
														{
																Data_433M[Data_433M_Len] = Rec_433M;
																Data_433M_Len ++;
																if(Data_433M_Len == DATA_433M_LEN)
																{
																		Status_433M |= 0x20;
																		Status_433M &= ~(1<<3);			
																}																	
														}
														data_time_L = 0;
														data_time_H = 0;
												}
												else if( ((time_H > DATA_TIME_MAX) && (data_time_H > DATA_TIME_MAX) )/* || ( (time_H > 60000) && (data_time_H > 60000))*/ )
												{
														Status_433M = 0;
														Deviation_Time = 0;
														start_flag_H = 0;
														start_time_H = 0;
														count_433M = 0;
														Data_433M_Len = 0;
														memset(Data_433M,0,sizeof(Data_433M));
												}
										}
										else
										{
												if( !(Status_433M & 0x8)	)        
												{ 
														if( (Data_433M[1] == 0x41) || (Data_433M[6] == 0x41) )
														{
																Data_433M[4] |= 1;			
																if( Data_433M[4] == 0x55 )
																{
																
																}
																else
																{
																		Data_433M[5] = Data_433M[0];
																		Data_433M[6] = Data_433M[1];
																		Data_433M[7] = Data_433M[2];
																		Data_433M[8] = Data_433M[3];
															//			Data_433M[9] = (0x0f - (Data_433M[4] & 0x0f)) | (Data_433M[4] & 0xf0);
																		Data_433M[9] = ( ~Data_433M[4] & 0x0f ) |(Data_433M[4] & 0xf0);
																}
																LED = !LED;
																Learn_Flag1 = 1;
																			USART_SendData(USART1,Data_433M[0]);
																while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
															
																	USART_SendData(USART1,Data_433M[1]);
																while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
															
																	USART_SendData(USART1,Data_433M[2]);
																while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
																	USART_SendData(USART1,Data_433M[3]);
																while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
															
																	USART_SendData(USART1,Data_433M[4]);
																while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
													
																	USART_SendData(USART1,Data_433M[5]);
																while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
															
																	USART_SendData(USART1,Data_433M[6]);
																while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
															
																	USART_SendData(USART1,Data_433M[7]);
																while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
																	USART_SendData(USART1,Data_433M[8]);
																while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
															
																	USART_SendData(USART1,Data_433M[9]);
																while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
															
														}				
														Status_433M = 0;		
														start_time_H = 0;
														start_time_L = 0;
														start_flag_H = 0;
														start_flag_L = 0;
														data_time_H = 0;
														data_time_L = 0;
														data_flag_H = 0;
														data_flag_L = 0;
														Deviation_Time = 0;
														count_433M = 0;
														Data_433M_Len = 0;
														//memset(Data_433M,0,sizeof(Data_433M));
														delay_ms(500);
												}						
										}
								}
						}	
				}
			
		}
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update | TIM_IT_CC3);	 	  
}




























void BEEP_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	 	
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStructure);
	
		GPIO_ResetBits(GPIOA,GPIO_Pin_8);
}

void Buzzer(int ms)
{
		BEEP= 1;
		delay_ms(ms);
		BEEP= 0;
		delay_ms(ms);
}

void LED_Light(void)
{
		if(Led_Flag == 1)
		{
				LED0 = 0;
				delay_ms(50);
				LED0 = 1;
		}
		else
		{
				LED0 = 1;
				delay_ms(50);
				LED0 = 0;
		}
}

void CharToHex(char * dest, char * buffer , int len)
{
	int i=0;
	int j=0;
	unsigned char temp;
	while(i<len)
	{
		temp=buffer[i];
		if((temp>=0x30)&&(temp<=0x39))
		{
			temp=temp-0x30;
			dest[j]=temp<<4;
		}
		else if((temp>=0x41)&&(temp<=0x46))
		{
			temp=temp-0x41+0x0A;
			dest[j]=temp<<4;
		}
		else if((temp>=0x61)&&(temp<=0x66))
		{
			temp=temp-0x61+0x0A;
			dest[j]=temp<<4;
		}
		else
		{
			dest[j]=0x00;
		}
		temp=buffer[i+1];
		if((temp>=0x30)&&(temp<=0x39))
		{
			temp=temp-0x30;
			dest[j]=dest[j]|temp;
		}
		else if((temp>=0x41)&&(temp<=0x46))
		{
			temp=temp-0x41+0x0A;
			dest[j]=dest[j]|temp;
		}
		else if((temp>=0x61)&&(temp<=0x66))
		{
			temp=temp-0x61+0x0A;
			dest[j]=dest[j]|temp;
		}
		else
		{
			dest[j]=dest[j]|0x00;
		}
		i=i+2;
		j=j+1;
	}
	dest[j-2]=dest[j-2]|0x0a;
	return;
} 
 
