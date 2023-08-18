typedef enum {
	LED_OFF,LED_ON
}LedStatus_t;
typedef enum{
	LED12,LED13,LED14,LED15
}LedNum_t;

void LedInits();
void LedCtrl(LedNum_t LedNum,LedStatus_t LedStatus);