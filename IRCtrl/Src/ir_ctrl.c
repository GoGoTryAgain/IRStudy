#include "ir_ctrl.h"
#include <string.h>
#include "tim.h"




uint16_t IRHSendbuf[200] = {0};
uint16_t IRLSendbuf[200] = {0};

static ir_data_t IR_data = {0};




void SaveData2Flash(uint32_t Adress,uint16_t *data,int32_t len)
{
	uint32_t offset = 0;
	uint32_t PageError = 0;
	uint16_t *dataPtr = data;
	
	PageError = HAL_FLASH_Unlock();
	
	FLASH_EraseInitTypeDef f;
	
	f.TypeErase = FLASH_TYPEERASE_PAGES;
	f.PageAddress = Adress;
	f.NbPages = 1;

	HAL_FLASHEx_Erase(&f,&PageError);
	
	while(len > 0)
	{
		PageError = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,Adress + offset,*dataPtr++);
		len -= 2;
		offset += 2;
	}
	HAL_FLASH_Lock();
		
}


void ReadDataFromFlash(uint32_t Adress,uint16_t *buf,int32_t len)
{
	uint32_t offset = 0;
	while(len > 0)
	{
		*buf ++ = *(__IO uint16_t*)(Adress + offset);
		len -= 2;
		offset += 2;
	}
}


void SendIR(uint8_t Level,uint32_t IRtimeus)
{
		
	if(Level)
	{
		HAL_Delay(IRtimeus / 10);  
	}
	else
	{
		HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
		HAL_Delay(IRtimeus / 10);  
		HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);
	}
	
}



void TestIrSend(void)
{
	uint8_t i = 0;
	
	ReadDataFromFlash(USER_ADDRESS,IR_data.IRHtimeBuf,sizeof(IRHSendbuf) + sizeof(IRLSendbuf));
	for(i = 0;IR_data.IRLtimeBuf[i] != 0 ;i++)
	{
		SendIR(0,IR_data.IRLtimeBuf[i]);
		SendIR(1,IR_data.IRHtimeBuf[i]);
		
	}
}

void IR_init(void)
{
	
	IR_data.IRHtimeBuf = IRHSendbuf;
	IR_data.IRLtimeBuf = IRLSendbuf;
	
}

void AddIRData(uint8_t level,uint16_t IRdata)
{
	if(level)
	{
		*IR_data.IRHtimeBuf ++ = IRdata;
	}
	else
	{
		*IR_data.IRLtimeBuf ++ = IRdata;
	}
	
	
}

void SaveThisIrData()
{
	IR_data.IRHtimeBuf = IRHSendbuf;
	IR_data.IRLtimeBuf = IRLSendbuf;
	SaveData2Flash(USER_ADDRESS,IR_data.IRHtimeBuf,sizeof(IRHSendbuf) + sizeof(IRLSendbuf));
	
	memset(IRHSendbuf,0,sizeof(IRHSendbuf));
	memset(IRHSendbuf,0,sizeof(IRLSendbuf));
	
}












