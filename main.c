#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include "semphr.h"
#include "led.h"
#include "log.h"
#include "temperature.h"

#define READ_TEMPERATURE_COMPLETE_ENVENT (1<<0)
#define READ_TEMPERATURE_ERROR_EVENT (1<<1)

void vApplicationMallocFailedHook(){}
void vApplicationGetIdleTaskMemory(StaticTask_t ** ppxIdleTaskTCBBuffer,
                                        StackType_t ** ppxIdleTaskStackBuffer,
                                        uint32_t * pulIdleTaskStackSize ){}
void vApplicationTickHook(){}
void vApplicationStackOverflowHook(TaskHandle_t xTask,
                                        char * pcTaskName ){}
void vApplicationIdleHook(){}
void vApplicationGetTimerTaskMemory(){}
void SystemInit(){}


void function1(void* parameter){
	 while(1){
LedCtrl(LED15,LED_ON);
vTaskDelay(1000);
LedCtrl(LED15,LED_OFF);
vTaskDelay(1000);
    }
}

void function2 (void* parameter){
	 while(1){
LedCtrl(LED14,LED_ON);
vTaskDelay(1500);
LedCtrl(LED14,LED_OFF);
vTaskDelay(1500);
    }
}


QueueHandle_t temperature_queue;
EventGroupHandle_t temperature_event;
QueueHandle_t lock_mutex;

void send_temperature(void* parameter){
	while(1){
	xEventGroupWaitBits(temperature_event,READ_TEMPERATURE_COMPLETE_ENVENT,pdTRUE,pdFALSE,10000);
	float buff = 0.0f;	
		//read temperature from queue
	UBaseType_t num_data =uxQueueMessagesWaiting(temperature_queue);

	xSemaphoreTake(lock_mutex,10000);//take lock mutex in 10s

	my_log("Temperature : [");
		for(int i=0;i<num_data;i++){
		xQueueReceive(temperature_queue,&buff,100000);
		my_log("%.2f, ",buff);
		}
	my_log("\b\b]\r\n");
	xSemaphoreGive(lock_mutex);	//back lock_mutex 
	}
}

void read_temperature(void* parameter){
	int count=0;
	while(1){
		//write temperature into queue
		float temperature = TEMP_MEASURE();
		xQueueSend(temperature_queue,&temperature, 10000 );
		vTaskDelay(100);
	if(count++>=10){
	count=0;
	xEventGroupSetBits(temperature_event,READ_TEMPERATURE_COMPLETE_ENVENT);
	}
	}
}

void send_log(){
	while(1){
		xSemaphoreTake(lock_mutex,10000);//take lock mutex in 10s
		my_log("[%d] - FreeRTOS \r\n",xTaskGetTickCount());
		xSemaphoreGive(lock_mutex);	//back lock_mutex 
		vTaskDelay(1000);
	}
}

void main()
{	ADC_INIT();
	USART2_INIT();
    LedInits();
	TaskHandle_t task1;
	TaskHandle_t task2;
	TaskHandle_t task3;
	TaskHandle_t task4;
	TaskHandle_t task5;

	temperature_queue = xQueueCreate(128,sizeof(float));
	temperature_event= xEventGroupCreate();
	lock_mutex = xSemaphoreCreateMutex();
xTaskCreate(function1,"task 1",1024,NULL,0,&task1);
xTaskCreate(function2,"task 2",1024,NULL,1,&task2);
xTaskCreate(send_temperature,"task 3",1024,NULL,1,&task3);
xTaskCreate(read_temperature,"task 4",2048,NULL,1,&task4);
xTaskCreate(send_log,"task 5",2048,NULL,1,&task5);
vTaskStartScheduler();
    while(1){

    }
}