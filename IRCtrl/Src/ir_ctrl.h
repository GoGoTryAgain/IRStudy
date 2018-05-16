#ifndef IR_CTRL__H
#define IR_CTRL__H


#include "stdint.h"



#define USER_ADDRESS	(uint32_t)0x08010000


typedef struct{
	
	uint32_t Num;
	uint16_t *IRHtimeBuf;
	uint16_t *IRLtimeBuf;
}ir_data_t;



void IR_init(void);
void TestIrSend(void);
void SaveThisIrData(void);
void AddIRData(uint8_t level,uint16_t IRdata);

void SaveData2Flash(uint32_t Adress,uint16_t *data,int32_t len);
void ReadDataFromFlash(uint32_t Adress,uint16_t *buf,int32_t len);

#endif
