#define LED_PERIOD_MAX 200
#define AUTOMATIC_BREATHING											0			//自动呼吸		默认
#define FIXED_DUTY_CYCLE											1			//固定占空比
typedef struct{
	uint8_t ledPeriod;			//LED 整个周期
	uint8_t ledPlus;			//LED 输出占空比
	uint8_t ledModeFlag;		//LED 输出模式
	uint8_t LedMode;			//LED 模式	自动呼吸/固定占空比
	uint8_t Last_LedMode;		//上一次 LED 模式
	uint8_t Ledduty_cycle;		//LED占空比
}_ledFun;

_ledFun ledFun;

/*
	50us中断
*/
//上电初始化一下模式
ledFun.LedMode = AUTOMATIC_BREATHING;	//自动呼吸
//占空比修改
//ledFun.Ledduty_cycle		 从自动呼吸模式转到固定占空比  需要从完全灭 逐渐里亮到固定值
void timeIsr(void){
	if(ledFun.Last_LedMode != ledFun.LedMode){
		if(ledFun.LedMode == AUTOMATIC_BREATHING){	//自动呼吸
			ledFun.ledPeriod = 0;
			ledFun.ledModeFlag = 0;
			ledFun.ledPlus = 0;
			ledFun.Ledduty_cycle = 0;
		}else if(ledFun.LedMode == FIXED_DUTY_CYCLE){
			ledFun.ledPeriod = 0;
			ledFun.ledPlus = 0;
		}
		ledFun.Last_LedMode = ledFun.LedMode;
	}
	if(++ledFun.ledPeriod >= LED_PERIOD_MAX){		//整个周期结束
		ledFun.ledPeriod = 0;
		if((ledFun.ledModeFlag == 1) && (ledFun.LedMode == AUTOMATIC_BREATHING)){	//渐灭
			if(ledFun.ledPlus){	//占空比越来越小
				--ledFun.ledPlus;
				if(!ledFun.ledPlus)
					ledFun.ledModeFlag = 0;
			}
		}else{	//渐亮
			if(ledFun.LedMode == AUTOMATIC_BREATHING){	//自动呼吸
				if(++ledFun.ledPlus == LED_PERIOD_MAX)	//占空比越来越大
					ledFun.ledModeFlag = 1;
			}else if(ledFun.LedMode == FIXED_DUTY_CYCLE){	//固定占空比
				if(ledFun.ledPlus < ledFun.Ledduty_cycle)
					ledFun.ledPlus++;
			}
		}
	}
 if(ledFun.ledPeriod < ledFun.ledPlus){	//1输出占空比
	 P1^0 = 1;
	 P1^1 = 1;
	 P1^2 = 1;
 }else{
	 P1^0 = 0;
	 P1^1 = 0;
	 P1^2 = 0;			 
 }
}