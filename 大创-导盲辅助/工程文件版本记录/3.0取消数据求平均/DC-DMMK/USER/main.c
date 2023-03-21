#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "hcsr04.h"
#include "lcd.h"
#include "beep.h"
#include "led.h"

float Distance=0;    //����������1����
float Distance1=0;   //����������1���ң�
float Distance2=0;   //����������1���У�
int main(void)//������
{		
	delay_init();	       //��ʱ����ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�ж����ȼ�NVIC
	uart_init(115200);	 //���ڱ�����
	LCD_Init();          //LCD��ʼ��
	LED_Init();          //LED��ʼ��
	BEEP_Init();         //BEEP��ʼ��
	Hcsr04Init();	       //HC-SR04��ʼ��
	POINT_COLOR=RED;	   //������ɫ
	LCD_ShowString(30,40,210,24,24,"Left distance:");
	LCD_ShowString(120,70,210,24,24," cm");
	LCD_ShowString(30,100,210,24,24,"Right distance:");
	LCD_ShowString(120,130,210,24,24," cm");
	LCD_ShowString(30,160,210,24,24,"Middle distance:");
	LCD_ShowString(120,190,210,24,24," cm");
	LED0=1;//��������LEDС�Ƶ��߼���ϵ
	LED1=~LED0;
	BEEP=LED1;
  while(1)
	{
			Distance = (Hcsr04GetLength());//�Ծ���������и�ֵ����
			Distance1 = (Hcsr04GetLength1());//�Ծ���������и�ֵ���ң�
			Distance2 = (Hcsr04GetLength2());//�Ծ���������и�ֵ���У�
			LCD_ShowNum(30,70,Distance,5,24);
			LCD_ShowNum(30,130,Distance1,5,24);
			LCD_ShowNum(30,190,Distance2,5,24);
			if((Distance<=75.0||Distance1<=75.0||Distance2<=75.0) && BEEP==0){     //�߼���ϵ
				BEEP=1;
				LED1=BEEP;
				LED0=~LED1;
			}else	if((Distance>75.0&&Distance1>75.0&&Distance2>75.0) && BEEP==1){  //�߼���ϵ��ʹ��else	if����һ���жϣ�
				BEEP=0;
				LED1=BEEP;
				LED0=~LED1;
			}
      delay_ms(100);//��ʱ����
	}
}	

