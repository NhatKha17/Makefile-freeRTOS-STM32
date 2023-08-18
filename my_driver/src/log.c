#include <stdint.h>
#include "stdio.h"
#include "stdarg.h"
#include "log.h"
#include <string.h>
char RX_BUF[32]={0};
static void DMA_USART2_INIT(){

		uint32_t* RCC_AHB1ENR=(uint32_t*)(0x40023830);
	*RCC_AHB1ENR|=(1<<21);//enable clock for DMA1
	/*TODO : NOTE SETUP DMA FOR ALL CHIP
	 * 1 -source address
	 * 2 -DST address
	 * 3 -size of data
	 */
	uint32_t* DMA1_S5CR = (uint32_t*)(0x40026000+0x10+0x18*5 );
	*DMA1_S5CR&=~1;//reset DMA
	// select channel 4 stream 5
	*DMA1_S5CR|= (0b100<<25);
	*DMA1_S5CR|=(1<<10);
	//1- Peripheral address : DMA1 read data from address UART_DR 0x40004400+0x04
		uint32_t* DMA1_S5PAR =(uint32_t*)(0x40026000+0x18+0x18*5);
				*DMA1_S5PAR=0x40004400+0x04;
	//2- save address : DMA1 write date to address Array user
		uint32_t* DMA1_S5MOAR =(uint32_t*)(0x40026000+0x1c+0x18*5);
				*DMA1_S5MOAR=(uint32_t)RX_BUF;
	//3- Number of data items
		uint32_t* DMA1_S5NDTR =(uint32_t*)(0x40026000+0x14+0x18*5);
				* DMA1_S5NDTR=sizeof(RX_BUF);
	//enable DMA1 stream 5
				* DMA1_S5CR|=(1<<0)|(1<<8);
}
void USART2_INIT(){
	/*USART2 USE GPIOA SO INIT GPIOA*/

    	uint32_t* RCC_AHB1ENR=(uint32_t*)(0x40023830);
	*RCC_AHB1ENR|=(1<<0);//enable clock for GPIOA
        uint32_t* RCC_APB2ENR=(uint32_t*)(0x40023840);
	*RCC_APB2ENR|=(1<<17);//enable clock for USART2

	uint32_t* GPIOA_MODER=(uint32_t*)(0x40020000+0x00);
	*GPIOA_MODER|=(0b10<<4)|(0b10<<6);//set GPIOA at alternate function PA2,PA3 set on 10
	uint32_t* GPIOA_AFAL=(uint32_t*)(0x40020000+0x20);
	*GPIOA_AFAL|=(0b0111<<8)|(0b0111<<12);//set PA2 - U2Tx and PA3 - U2Rx 

	uint32_t* USART_BRR=(uint32_t*)(0x40004400+0x08);//CREAT BRR
	//SET BAUDRATE
	/*
		 * baud rate = fck/(16*UARTDIV)
		 * SET 9600=fck/(16*UARTDIV)=>9600=16mhz/(16*UARTDIV)=>UARTDIV=104,167
		 * mintassa =104
		 * franction = 0.167*16 = 3
		 */
	*USART_BRR=(104<<4)|(3<<0);

	//set frame date
	uint32_t* USART_CR1=(uint32_t*)(0x40004400+0x0C);
	*USART_CR1&=~(1<<12);
	*USART_CR1&=~(1<<10);

	//TODO: enable DMA for Rx
	uint32_t* USART_CR3=(uint32_t*)(0x40004400+0x14);
	*USART_CR3|=(1<<6);
	DMA_USART2_INIT();
	/*FINALLY ENABLE USART 	ENABLE TRANMITTER  enable receiver*/
	*USART_CR1|=(1<<2)|(1<<3)|(1<<13);
}

static void USART2_SEND_DATA(char _data){
	uint32_t* USART_DR=(uint32_t*)(0x40004400+0x04);
    uint32_t* USART_SR=(uint32_t*)(0x40004400+0x00);
	//check data: transmission complete by read bit 13 in SR
	while(((*USART_SR>>7)&1)==0);
    *USART_DR = _data;
	while(((*USART_SR>>6)&1)==0);
	*USART_SR&=~(1<<6);//CLEAR BIT 6 IN SR
}
void my_log(char* format, ...){
    va_list arg;
    va_start(arg, format);
    char send_buf[128]={0};
    vsprintf(send_buf,format,arg);
    va_end(arg);
    int msg_len =strlen(send_buf); 
    for(int i=0;i<msg_len;i++){
        USART2_SEND_DATA(send_buf[i]);
    }
}

uint8_t UART2_RECV_DATA(){
	uint32_t* UART_SR = (uint32_t*)(0x40004400+0x00);
	uint32_t* UART_DR = (uint32_t*)(0x40004400+0x04);
	while(((*UART_SR>>5)&1)==0);
	return *UART_DR;
}


