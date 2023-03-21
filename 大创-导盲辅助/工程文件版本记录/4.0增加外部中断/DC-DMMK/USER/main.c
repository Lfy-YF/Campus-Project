#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "hcsr04.h"
#include "lcd.h"
#include "beep.h"
#include "led.h"

float Distance=0;    //定义距离变量1（左）
float Distance1=0;   //定义距离变量1（右）
float Distance2=0;   //定义距离变量1（中）
int main(void)//主函数
{		
	delay_init();	       //延时器初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //中断优先级NVIC
	uart_init(115200);	 //串口比特率
	LCD_Init();          //LCD初始化
	LED_Init();          //LED初始化
	BEEP_Init();         //BEEP初始化
	Hcsr04Init();	       //HC-SR04初始化
	POINT_COLOR=RED;	   //画笔颜色
	LCD_ShowString(30,40,210,24,24,"Left distance:");
	LCD_ShowString(120,70,210,24,24," cm");
	LCD_ShowString(30,100,210,24,24,"Right distance:");
	LCD_ShowString(120,130,210,24,24," cm");
	LCD_ShowString(30,160,210,24,24,"Middle distance:");
	LCD_ShowString(120,190,210,24,24," cm");
	LED0=1;//蜂鸣器与LED小灯的逻辑关系
	LED1=~LED0;
	BEEP=LED1;
  while(1)
	{
			Distance = (Hcsr04GetLength());//对距离变量进行赋值（左）
			Distance1 = (Hcsr04GetLength1());//对距离变量进行赋值（右）
			Distance2 = (Hcsr04GetLength2());//对距离变量进行赋值（中）
			LCD_ShowNum(30,70,Distance,5,24);
			LCD_ShowNum(30,130,Distance1,5,24);
			LCD_ShowNum(30,190,Distance2,5,24);
			if((Distance<=75.0||Distance1<=75.0||Distance2<=75.0) && BEEP==0){     //逻辑关系
				BEEP=1;
				LED1=BEEP;
				LED0=~LED1;
			}else	if((Distance>75.0&&Distance1>75.0&&Distance2>75.0) && BEEP==1){  //逻辑关系（使用else	if减少一次判断）
				BEEP=0;
				LED1=BEEP;
				LED0=~LED1;
			}
      delay_ms(100);//延时设置
	}
}	

