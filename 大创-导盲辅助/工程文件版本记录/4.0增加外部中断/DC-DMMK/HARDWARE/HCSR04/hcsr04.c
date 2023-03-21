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

u16 msHcCount = 0;  //����������

void Hcsr04Init()
{      
    GPIO_InitTypeDef GPIO_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		EXTI_InitTypeDef EXTI_InitStructure;
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	
    RCC_APB2PeriphClockCmd(HCSR04_CLK, ENABLE);
     
    //IO�ڳ�ʼ�� (���ƶ�)  
    GPIO_InitStructure.GPIO_Pin =HCSR04_TRIG|HCSR04_TRIG1|HCSR04_TRIG2;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //����IO�����ģʽΪ�������
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(HCSR04_PORT,HCSR04_TRIG|HCSR04_TRIG1|HCSR04_TRIG2);
	
    //IO�ڳ�ʼ�������նˣ�
    GPIO_InitStructure.GPIO_Pin =   HCSR04_ECHO|HCSR04_ECHO1|HCSR04_ECHO2;     
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //����IO������ģʽΪ��������
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);  
		GPIO_ResetBits(HCSR04_PORT,HCSR04_ECHO|HCSR04_ECHO1|HCSR04_ECHO2);
	
		//�������ж�
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
		
		//HCSR04_ECHO
	  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;  //HCSR04_ECHO
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	  NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
		
		EXTI_InitStructure.EXTI_Line = EXTI_Line1;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//�����½��ش���
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ���ⲿ�ж�
		EXTI_Init(&EXTI_InitStructure);
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOF,GPIO_PinSource1);
		
		//HCSR04_ECHO1
	  NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;  //HCSR04_ECHO1
	  NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
		
		EXTI_InitStructure.EXTI_Line = EXTI_Line3;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//�����½��ش���
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ���ⲿ�ж�
		EXTI_Init(&EXTI_InitStructure);
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOF,GPIO_PinSource3);
		
		//HCSR04_ECH2
	  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;  //HCSR04_ECHO2
	  NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
		
		EXTI_InitStructure.EXTI_Line = EXTI_Line5;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//�����½��ش���
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ���ⲿ�ж�
		EXTI_Init(&EXTI_InitStructure);
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOF,GPIO_PinSource5);
		//�������ж�
		
	
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
 
void OpenTimer()   //�򿪶�ʱ��3      
{
	TIM_SetCounter(TIM3,0);  //����TIM3�����Ĵ�����ֵΪ0
	msHcCount = 0;   
	TIM_Cmd(TIM3, ENABLE);  //ʹ�ܶ�ʱ��TIM3
}
 
void CloseTimer() //�رն�ʱ��3       
{
	TIM_Cmd(TIM3, DISABLE);     //ֹͣʹ�ܶ�ʱ��TIM3	
}
 
 
//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)   
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���TIM3�ĸ����жϷ������
	{
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );   //���TIM3�����жϱ�־        
			msHcCount++; //��������ʼ��
	}
}

//HCSR04_ECH0
void EXTI1_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line1))
	{
		//���жϴ���
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}

//HCSR04_ECH1
void EXTI3_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line3))
	{
		//���жϴ���
		EXTI_ClearITPendingBit(EXTI_Line3);
	}
}

//HCSR04_ECH2
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line5))
	{
		//���жϴ���
		EXTI_ClearITPendingBit(EXTI_Line5);
	}
}

u32 GetEchoTimer(void)    //��ȡ��ʱ��3������ֵ
{
	u32 t = 0;
	t = msHcCount*1000;     //��ʱ��ת��Ϊ΢��
	t += TIM_GetCounter(TIM3);   //��ȡ��ǰ�Ĵ�����ֵ
	TIM3->CNT = 0;  //���������
	delay_ms(50);
	return t;
}

//����20us�ĸߵ�ƽ
void HC_SR04_Control()
{
	TRIG_Send = 1;     //PF0�������ƶ˸ߵ�ƽ��
	TRIG_Send1 = 1;    //PF2�������ƶ˸ߵ�ƽ��
	TRIG_Send2 = 1;    //PF4�������ƶ˸ߵ�ƽ��
	delay_us(20);
	TRIG_Send = 0;     //PF0��������ģ���ѿ�ʼ����8��40khz���壩
	TRIG_Send1 = 0;    //PF2��������ģ���ѿ�ʼ����8��40khz���壩
	TRIG_Send2 = 0;    //PF4��������ģ���ѿ�ʼ����8��40khz���壩
}
 
float Hcsr04GetLength(void )//��೬�������
{
	u32 t = 0;
	float lengthTemp = 0;
		HC_SR04_Control();
		while(ECHO_Reci == 0);    //��ECHO_ReciΪ�͵�ƽ����һֱѭ����ֱ��Ϊ�ߵ�ƽ 
		OpenTimer();              //��ʱ˵����⵽�ߵ�ƽ��������ʱ������ʼ��ʱ   
		while(ECHO_Reci == 1);    //��ECHO_ReciΪ�ߵ�ƽ����һֱѭ����ֱ��Ϊ�͵�ƽ
		CloseTimer();             //��ʱ˵����⵽�͵�ƽ���رն�ʱ����ֹͣ��ʱ  
		t = GetEchoTimer();       //��ȡ��ʱ��ʱ��
		lengthTemp = ((float)t/58.0);   //���㵥��������루���ݴ���ת����cm��
	return lengthTemp;
}
 
float Hcsr04GetLength1(void )//�Ҳ೬�������
{
	u32 t = 0;
	float lengthTemp = 0;
		HC_SR04_Control();
		while(ECHO_Reci1 == 0);   //��ECHO_ReciΪ�͵�ƽ����һֱѭ����ֱ��Ϊ�ߵ�ƽ 
		OpenTimer();              //��ʱ˵����⵽�ߵ�ƽ��������ʱ������ʼ��ʱ   
		while(ECHO_Reci1 == 1);   //��ECHO_ReciΪ�ߵ�ƽ����һֱѭ����ֱ��Ϊ�͵�ƽ
		CloseTimer();             //��ʱ˵����⵽�͵�ƽ���رն�ʱ����ֹͣ��ʱ  
		t = GetEchoTimer();       //��ȡ��ʱ��ʱ��
		lengthTemp = ((float)t/58.0);   //���㵥��������루���ݴ���ת����cm��
	return lengthTemp;
}
 
float Hcsr04GetLength2(void )//�м䳬�������
{
	u32 t = 0;
	float lengthTemp = 0;
		HC_SR04_Control();
		while(ECHO_Reci2 == 0);   //��ECHO_ReciΪ�͵�ƽ����һֱѭ����ֱ��Ϊ�ߵ�ƽ 
		OpenTimer();              //��ʱ˵����⵽�ߵ�ƽ��������ʱ������ʼ��ʱ   
		while(ECHO_Reci2 == 1);   //��ECHO_ReciΪ�ߵ�ƽ����һֱѭ����ֱ��Ϊ�͵�ƽ
		CloseTimer();             //��ʱ˵����⵽�͵�ƽ���رն�ʱ����ֹͣ��ʱ  
		t = GetEchoTimer();       //��ȡ��ʱ��ʱ��
		lengthTemp = ((float)t/58.0);   //���㵥��������루���ݴ���ת����cm��
	return lengthTemp;
}
 

 






