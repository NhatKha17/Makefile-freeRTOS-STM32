#include <stdint.h>
#include "led.h"

void LedInits(){
	uint32_t* RCC_AHB1ENR=(uint32_t*)(0x40023830);
	*RCC_AHB1ENR|=(1<<3);//enable clock for GPIOD

	uint32_t* GPIOD_MODER=(uint32_t*) (0x40020C00+0x00);//creat led
	uint32_t* GPIOD_OTYPER=(uint32_t*) (0x40020C00+0x04);//creat mode push-pull or drain open
	*GPIOD_MODER&=~(0xFF<<24);//reset memory moder
	*GPIOD_MODER|=(0b01<<24)|(0b01<<26)|(0b01<<28)|(0b01<<30);// set moder 0 1 : mode  General purpose OUTPUT
	*GPIOD_OTYPER&=~(0b1111<<12);//set OTYPER 0: Output push-pull (reset state)
}

void LedCtrl(LedNum_t LedNum,LedStatus_t LedStatus){
	uint32_t* GPIOD_ODR=(uint32_t* )(0x40020C00+0x14);
	if(LedStatus==LED_ON){
		*GPIOD_ODR|=(1<<(12+LedNum));
	}
	else *GPIOD_ODR&=~(1<<(12+LedNum));
	/* USER CODE END 0 */
}