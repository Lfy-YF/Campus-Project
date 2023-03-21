#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "hcsr04.h"
#include "lcd.h"
#include "beep.h"
#include "led.h"

float Distance=0;

int main(void)
{		
	u32 i=0;
	delay_init();	 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	uart_init(115200);	 
	LCD_Init();
	LED_Init();
	BEEP_Init();
	Hcsr04Init();	
	POINT_COLOR=RED;	//»­±ÊÑÕÉ«
	LED0=1;
	LED1=~LED0;
	BEEP=LED1;
  while(1)
	{
			i++;
			Distance = (Hcsr04GetLength());
			if(Distance<=60.0 && BEEP==0){
				BEEP=1;
				LED1=BEEP;
				LED0=~LED1;
			}
			if(Distance>60.0 && BEEP==1){
				BEEP=0;
				LED1=BEEP;
				LED0=~LED1;
			}
			LCD_ShowString(30,40,210,24,24,"The distance is :");
			LCD_ShowNum(30,60,Distance,5,24);
			LCD_ShowString(120,60,210,24,24," cm");
      delay_ms(100);
			if(i==100)
			{
			  LCD_Clear(WHITE);
				POINT_COLOR=RED;
				i=0;
			}
	}
}	

