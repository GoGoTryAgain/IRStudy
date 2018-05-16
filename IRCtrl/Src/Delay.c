#include "Delay.h"
#include "stm32f10x.h"
#include "core_cm3.h"



//static uint8_t SysClK;


//systick clk = 72M





void delay_us(uint8_t nus)
{
	int32_t temp;
	SysTick->LOAD = 72 * nus;
	SysTick->VAL = 0x00;//Çå¿Õ
	SysTick->CTRL = 0x01;//Ê¹ÄÜ
	
	do
	{
		temp = SysTick->CTRL;
	}while((temp & 0x01) && (!(temp&(1<<16))));
	SysTick->CTRL = 0x00;
	SysTick->VAL = 0x00;
	
	
}
















