#include "hcsr04.h"
#include "sys.h"
#include "delay.h"

#define HCSR04_PORT     GPIOF  //GPIO(F)
#define HCSR04_CLK      RCC_APB2Periph_GPIOF

#define HCSR04_TRIG     GPIO_Pin_0//һ�ų��������󣩿��ƶ�����PF0
#define HCSR04_ECHO     GPIO_Pin_1//һ�ų��������󣩽��ն�����PF1

#define HCSR04_TRIG1     GPIO_Pin_2//���ų��������ң����ƶ�����PF2
#define HCSR04_ECHO1     GPIO_Pin_3//���ų��������ң����ն�����PF3
 
#define HCSR04_TRIG2     GPIO_Pin_4//���ų��������У����ƶ�����PF4
#define HCSR04_ECHO2     GPIO_Pin_5//���ų��������У����ն�����PF5
 
#define TRIG_Send  PFout(0)  //һ�ų��������󣩿��ƶ�����PF0
#define ECHO_Reci  PFin(1)   //һ�ų��������󣩽��ն�����PF1

#define TRIG_Send1  PFout(2) //���ų��������ң����ƶ�����PF2
#define ECHO_Reci1  PFin(3)  //���ų��������ң����ն�����PF3

#define TRIG_Send2  PFout(4) //���ų��������У����ƶ�����PF4
#define ECHO_Reci2  PFin(5)  //���ų��������У����ն�����PF5

u16 msHcCount = 0;

void Hcsr04Init()
{      
    GPIO_InitTypeDef GPIO_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	
    RCC_APB2PeriphClockCmd(HCSR04_CLK, ENABLE);
     
    //IO�ڳ�ʼ�� (��)  
    GPIO_InitStructure.GPIO_Pin =HCSR04_TRIG;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //����IO�����ģʽΪ�������
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(HCSR04_PORT,HCSR04_TRIG);
	
    //IO�ڳ�ʼ������
    GPIO_InitStructure.GPIO_Pin =   HCSR04_ECHO;     
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //����IO������ģʽΪ��������
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);  
		GPIO_ResetBits(HCSR04_PORT,HCSR04_ECHO);	
 
	  //IO�ڳ�ʼ��1���ң�
    GPIO_InitStructure.GPIO_Pin =HCSR04_TRIG1;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //����IO�����ģʽΪ�������
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(HCSR04_PORT,HCSR04_TRIG1);
	
    //IO�ڳ�ʼ��1���ң�
    GPIO_InitStructure.GPIO_Pin =   HCSR04_ECHO1;     
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //����IO������ģʽΪ��������
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);  
		GPIO_ResetBits(HCSR04_PORT,HCSR04_ECHO1);	
		
		//IO�ڳ�ʼ��2���У�
    GPIO_InitStructure.GPIO_Pin =HCSR04_TRIG2;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //����IO�����ģʽΪ�������
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(HCSR04_PORT,HCSR04_TRIG2);
	
    //IO�ڳ�ʼ��2���У�
    GPIO_InitStructure.GPIO_Pin =   HCSR04_ECHO2;     
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //����IO������ģʽΪ��������
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);  
		GPIO_ResetBits(HCSR04_PORT,HCSR04_ECHO2);	

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);   //ʹ�ܶ�ʱ��ʱ��

		TIM_TimeBaseStructure.TIM_Period = (1000-1); //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
		TIM_TimeBaseStructure.TIM_Prescaler =(72-1); //����������ΪTIM3ʱ��Ƶ�ʳ�����Ԥ��Ƶ            
		TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;      //����ʱ�ӷָ�:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 	 
		
		TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);    //ʹ��ָ����TIM3�ж�,��������ж�
		
		//�ж����ȼ�NVIC����
	  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	  NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
		
		TIM_Cmd(TIM3,DISABLE);     
}
 
void OpenTimer()       
{
	TIM_SetCounter(TIM3,0);  //����TIM3�����Ĵ�����ֵΪ0
	msHcCount = 0;   
	TIM_Cmd(TIM3, ENABLE);  //ʹ�ܶ�ʱ��TIM3
}
 
void CloseTimer()        
{
	TIM_Cmd(TIM3, DISABLE);     //ֹͣʹ�ܶ�ʱ��TIM3	
}
 
 
//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)   
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���TIM3�ĸ����жϷ������
	{
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );   //���TIM3�����жϱ�־        
			msHcCount++;
	}
}
 
u32 GetEchoTimer(void)
{
	u32 t = 0;
	t = msHcCount*1000;     //��ʱ��ת��Ϊ΢��
	t += TIM_GetCounter(TIM3);   //��ȡ��ǰ�Ĵ�����ֵ
	TIM3->CNT = 0;  //���������
	delay_ms(50);
	return t;
}
 
float Hcsr04GetLength(void )//��೬�������
{
	u32 t = 0;
	int i = 0;
	float lengthTemp = 0;
	float sum = 0;
	//while(i!=2)
	//{
		TRIG_Send = 1;    //PF0���һ���ߵ�ƽ   
		delay_us(20);
		TRIG_Send = 0;    //PF1���һ���͵�ƽ
		while(ECHO_Reci == 0);     
		OpenTimer();   //��ʼ��ʱ   
		i = i + 1;
		while(ECHO_Reci == 1);
		CloseTimer();     //������ʱ 
		t = GetEchoTimer();        
		lengthTemp = ((float)t/58.0);   //���㵥���������
		sum = lengthTemp + sum ;   
	//}
	lengthTemp = sum;//2.0; //����ξ����ƽ��ֵ
	return lengthTemp;
}
 
float Hcsr04GetLength1(void )//�Ҳ೬�������
{
	u32 t = 0;
	int i = 0;
	float lengthTemp = 0;
	float sum = 0;
	//while(i!=2)
	//{
		TRIG_Send1 = 1;    //PF2���һ���ߵ�ƽ   
		delay_us(20);
		TRIG_Send1 = 0;    //PF3���һ���͵�ƽ
		while(ECHO_Reci1 == 0);     
		OpenTimer();   //��ʼ��ʱ   
		i = i + 1;
		while(ECHO_Reci1 == 1);
		CloseTimer();     //������ʱ 
		t = GetEchoTimer();        
		lengthTemp = ((float)t/58.0);   //���㵥���������
		sum = lengthTemp + sum ;   
	//}
	lengthTemp = sum;//2.0; //����ξ����ƽ��ֵ
	return lengthTemp;
}
 
float Hcsr04GetLength2(void )//�м䳬�������
{
	u32 t = 0;
	int i = 0;
	float lengthTemp = 0;
	float sum = 0;
	//while(i!=2)
	//{
		TRIG_Send2 = 1;    //PF2���һ���ߵ�ƽ   
		delay_us(20);
		TRIG_Send2 = 0;    //PF3���һ���͵�ƽ
		while(ECHO_Reci2 == 0);     
		OpenTimer();   //��ʼ��ʱ   
		i = i + 1;
		while(ECHO_Reci2 == 1);
		CloseTimer();     //������ʱ 
		t = GetEchoTimer();        
		lengthTemp = ((float)t/58.0);   //���㵥���������
		sum = lengthTemp + sum ;   
	//}
	lengthTemp = sum;//2.0; //����ξ����ƽ��ֵ
	return lengthTemp;
}
 

 






