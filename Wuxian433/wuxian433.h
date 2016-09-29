#ifndef __WUXIAN433_H
#define __WUXIAN433_H
#include "sys.h"

#define MDATA      PBin(0)	 	    //433M���������
#define OUT_433M   PCout(12)	 	    //433M���������

extern uint8 Switch_ID[5];
void Init_433M(void);
void Send_433M(uint8* Data);
void Send_433M1(uint8 * Data);

#endif

