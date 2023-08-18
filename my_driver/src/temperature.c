#include "temperature.h"
#include <stdint.h>

void ADC_INIT(){
	uint32_t* RCC_APB2ENR=(uint32_t*)(0x40023844);
	*RCC_APB2ENR|=(1<<8);//enable clock for GPIOA
	
	uint32_t* ADC_SMPR1=(uint32_t*)0X4001200C;
	*ADC_SMPR1|=(0b111<<18);//select ADC1_IN16
	
	uint32_t* ADC_JSQR=(uint32_t*)0X40012038;
	*ADC_JSQR&=~(0b11<<20);//select 1 convertion
	*ADC_JSQR|=(16<<15);//selcect chennal 16 for JSQ4
	
	uint32_t* ADC_CCR=(uint32_t*)0X40012304;
	*ADC_CCR|=(1<<23);//enable temp sensor (TSVREFE)
	
	uint32_t* ADC_CR2=(uint32_t*)0X40012008;
	*ADC_CR2|=(1<<0);//enable ADC
}

static float ADC_MEASURE(){
	uint32_t* ADC_CR2=(uint32_t*)0X40012008;
	*ADC_CR2|=(1<<22);//start injected channel to measure
	
	uint32_t* ADC_SR=(uint32_t*)0X40012000;
	while(((*ADC_SR>>2)&1)==0);
	*ADC_SR&=~(1<<2);
	
	uint32_t* ADC_JDR1=(uint32_t*)0x4001203c;
	float read_sensor =(3.0*(*ADC_JDR1)/4095.0);
	return read_sensor;
}
float TEMP_MEASURE(){
	float vsense= ADC_MEASURE();
	const float v25=0.76;
	const float avg_slope =2.5/1000;
	float temp = ((vsense - v25)/avg_slope)+25;
    return temp;
}