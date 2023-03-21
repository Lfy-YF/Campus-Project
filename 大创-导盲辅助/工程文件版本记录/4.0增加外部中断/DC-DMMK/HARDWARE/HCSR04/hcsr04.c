#include "hcsr04.h"
#include "sys.h"
#include "delay.h"

#define HCSR04_PORT     GPIOF  //GPIO(F)
#define HCSR04_CLK      RCC_APB2Periph_GPIOF

#define HCSR04_TRIG     GPIO_Pin_0//一号超声波（左）控制端引脚PF0
#define HCSR04_ECHO     GPIO_Pin_1//一号超声波（左）接收端引脚PF1

#define HCSR04_TRIG1     GPIO_Pin_2//二号超声波（右）控制端引脚PF2
#define HCSR04_ECHO1     GPIO_Pin_3//二号超声波（右）接收端引脚PF3
 
#define HCSR04_TRIG2     GPIO_Pin_4//三号超声波（中）控制端引脚PF4
#define HCSR04_ECHO2     GPIO_Pin_5//三号超声波（中）接收端引脚PF5
 
#define TRIG_Send  PFout(0)  //一号超声波（左）控制端引脚PF0
#define ECHO_Reci  PFin(1)   //一号超声波（左）接收端引脚PF1

#define TRIG_Send1  PFout(2) //二号超声波（右）控制端引脚PF2
#define ECHO_Reci1  PFin(3)  //二号超声波（右）接收端引脚PF3

#define TRIG_Send2  PFout(4) //三号超声波（中）控制端引脚PF4
#define ECHO_Reci2  PFin(5)  //三号超声波（中）接收端引脚PF5

u16 msHcCount = 0;  //超声波计数

void Hcsr04Init()
{      
    GPIO_InitTypeDef GPIO_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		EXTI_InitTypeDef EXTI_InitStructure;
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	
    RCC_APB2PeriphClockCmd(HCSR04_CLK, ENABLE);
     
    //IO口初始化 (控制端)  
    GPIO_InitStructure.GPIO_Pin =HCSR04_TRIG|HCSR04_TRIG1|HCSR04_TRIG2;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //设置IO口输出模式为推挽输出
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(HCSR04_PORT,HCSR04_TRIG|HCSR04_TRIG1|HCSR04_TRIG2);
	
    //IO口初始化（接收端）
    GPIO_InitStructure.GPIO_Pin =   HCSR04_ECHO|HCSR04_ECHO1|HCSR04_ECHO2;     
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //设置IO口输入模式为浮空输入
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);  
		GPIO_ResetBits(HCSR04_PORT,HCSR04_ECHO|HCSR04_ECHO1|HCSR04_ECHO2);
	
		//新增加中断
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
		
		//HCSR04_ECHO
	  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;  //HCSR04_ECHO
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	  NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
		
		EXTI_InitStructure.EXTI_Line = EXTI_Line1;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//上升下降沿触发
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能外部中断
		EXTI_Init(&EXTI_InitStructure);
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOF,GPIO_PinSource1);
		
		//HCSR04_ECHO1
	  NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;  //HCSR04_ECHO1
	  NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
		
		EXTI_InitStructure.EXTI_Line = EXTI_Line3;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//上升下降沿触发
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能外部中断
		EXTI_Init(&EXTI_InitStructure);
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOF,GPIO_PinSource3);
		
		//HCSR04_ECH2
	  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;  //HCSR04_ECHO2
	  NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
		
		EXTI_InitStructure.EXTI_Line = EXTI_Line5;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//上升下降沿触发
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能外部中断
		EXTI_Init(&EXTI_InitStructure);
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOF,GPIO_PinSource5);
		//新增加中断
		
	
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);   //使能定时器时钟

		TIM_TimeBaseStructure.TIM_Period = (1000-1); //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
		TIM_TimeBaseStructure.TIM_Prescaler =(72-1); //设置用来作为TIM3时钟频率除数的预分频            
		TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;      //设置时钟分割:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //根据指定的参数初始化TIMx的时间基数单位
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 	 
		
		TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);    //使能指定的TIM3中断,允许更新中断
		
		//中断优先级NVIC设置
	  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	  NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
		
		TIM_Cmd(TIM3,DISABLE);     
}
 
void OpenTimer()   //打开定时器3      
{
	TIM_SetCounter(TIM3,0);  //设置TIM3计数寄存器的值为0
	msHcCount = 0;   
	TIM_Cmd(TIM3, ENABLE);  //使能定时器TIM3
}
 
void CloseTimer() //关闭定时器3       
{
	TIM_Cmd(TIM3, DISABLE);     //停止使能定时器TIM3	
}
 
 
//定时器3中断服务程序
void TIM3_IRQHandler(void)   
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查TIM3的更新中断发生与否
	{
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );   //清除TIM3更新中断标志        
			msHcCount++; //计数器开始加
	}
}

//HCSR04_ECH0
void EXTI1_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line1))
	{
		//做中断处理
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}

//HCSR04_ECH1
void EXTI3_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line3))
	{
		//做中断处理
		EXTI_ClearITPendingBit(EXTI_Line3);
	}
}

//HCSR04_ECH2
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line5))
	{
		//做中断处理
		EXTI_ClearITPendingBit(EXTI_Line5);
	}
}

u32 GetEchoTimer(void)    //获取定时器3计数器值
{
	u32 t = 0;
	t = msHcCount*1000;     //将时间转化为微秒
	t += TIM_GetCounter(TIM3);   //获取当前寄存器的值
	TIM3->CNT = 0;  //清零计数器
	delay_ms(50);
	return t;
}

//发送20us的高电平
void HC_SR04_Control()
{
	TRIG_Send = 1;     //PF0（给控制端高电平）
	TRIG_Send1 = 1;    //PF2（给控制端高电平）
	TRIG_Send2 = 1;    //PF4（给控制端高电平）
	delay_us(20);
	TRIG_Send = 0;     //PF0（超声波模块已开始发送8个40khz脉冲）
	TRIG_Send1 = 0;    //PF2（超声波模块已开始发送8个40khz脉冲）
	TRIG_Send2 = 0;    //PF4（超声波模块已开始发送8个40khz脉冲）
}
 
float Hcsr04GetLength(void )//左侧超声波测距
{
	u32 t = 0;
	float lengthTemp = 0;
		HC_SR04_Control();
		while(ECHO_Reci == 0);    //若ECHO_Reci为低电平，则一直循环，直到为高电平 
		OpenTimer();              //此时说明检测到高电平，开启定时器，开始计时   
		while(ECHO_Reci == 1);    //若ECHO_Reci为高电平，则一直循环，直到为低电平
		CloseTimer();             //此时说明检测到低电平，关闭定时器，停止计时  
		t = GetEchoTimer();       //获取定时器时间
		lengthTemp = ((float)t/58.0);   //计算单次物体距离（数据处理，转换成cm）
	return lengthTemp;
}
 
float Hcsr04GetLength1(void )//右侧超声波测距
{
	u32 t = 0;
	float lengthTemp = 0;
		HC_SR04_Control();
		while(ECHO_Reci1 == 0);   //若ECHO_Reci为低电平，则一直循环，直到为高电平 
		OpenTimer();              //此时说明检测到高电平，开启定时器，开始计时   
		while(ECHO_Reci1 == 1);   //若ECHO_Reci为高电平，则一直循环，直到为低电平
		CloseTimer();             //此时说明检测到低电平，关闭定时器，停止计时  
		t = GetEchoTimer();       //获取定时器时间
		lengthTemp = ((float)t/58.0);   //计算单次物体距离（数据处理，转换成cm）
	return lengthTemp;
}
 
float Hcsr04GetLength2(void )//中间超声波测距
{
	u32 t = 0;
	float lengthTemp = 0;
		HC_SR04_Control();
		while(ECHO_Reci2 == 0);   //若ECHO_Reci为低电平，则一直循环，直到为高电平 
		OpenTimer();              //此时说明检测到高电平，开启定时器，开始计时   
		while(ECHO_Reci2 == 1);   //若ECHO_Reci为高电平，则一直循环，直到为低电平
		CloseTimer();             //此时说明检测到低电平，关闭定时器，停止计时  
		t = GetEchoTimer();       //获取定时器时间
		lengthTemp = ((float)t/58.0);   //计算单次物体距离（数据处理，转换成cm）
	return lengthTemp;
}
 

 






