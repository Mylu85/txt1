//Author: Mr lu
//Project Start: 2025.1.2
//last updated:2025.1.2
//****************************************
//说明：
//teensy+futaba 可以完成实验

//本次实验使用的是NXP的i.mx rt1062 配合翼燃科技的keeli 1.01 PCB板
//teensy4.0有多个串口，keeli使用的是Serial5.

//软硬件：
//teensy 4.0 开发板 NXP i.MX RT1062
//接收机RSF08SB，遥控器FUTABA T14SG
//teeny 1.59.0 IDE aruino 2.3.4 

//imx/teensy 注意：
//一定要关闭杀毒软件，它会删除gcc文件和其他重要文件
//下载程序需要teensy.exe
//preference 里面也需要增加 package_teensy_index.json
//同时开发板管理器也需要安装Teensy. 


//****************************************
//引用头文件
//****************************************
#include "src/SBUS/SBUS.h" //注意一定要在 src 这个文件夹下
//****************************************
//宏定义
//****************************************
#define USE_SBUS_RX

#if defined USE_SBUS_RX
  SBUS sbus(Serial5);
  uint16_t sbusChannels[16];
  bool sbusFailSafe;
  bool sbusLostFrame;
#endif

//****************************************
//全局变量定义
//****************************************
int a = 0;//全局变量 作为开关使用

float dt;
unsigned long current_time, prev_time;
unsigned long print_counter, serial_counter;

unsigned long channel_1_pwm, channel_2_pwm, channel_3_pwm, channel_4_pwm, channel_5_pwm,channel_6_pwm;// channel_7_pwm;
unsigned long channel_1_pwm_prev, channel_2_pwm_prev, channel_3_pwm_prev, channel_4_pwm_prev;


//****************************************
//引脚定义
//****************************************

//接收机sbus信号端接ESP32 nano d9引脚

//****************************************
//准备工作做好， 各个功能端口打开
//****************************************
void setup() 
{

  Serial.begin(9600);
  delay(100);
 
  //打开串口接开场白
  Serial.println();
  delay(5000);
  Serial.println("***************************");
  delay(100);
  Serial.println("hi everyone!   ");
  delay(100);
  Serial.println();
  delay(100);
  delay(1000);
 
  //初始化RC
  sbus.begin();
}//end setup


//*********************************************
//主循环入口
//*********************************************
  void loop() 
{

  prev_time = current_time;      
  current_time = micros();      
  dt = (current_time - prev_time)/1000000.0;

  while(a==0)
  {
   Serial.println("Enter passcode to proceed.");   
   Serial.println("Waiting for key in ...");
   while(!(Serial.parseInt()==1));//软件锁（阻塞）防止运行直接跑起来   
   a=1;
   Serial.println("key ok, you can proceed... ");
  }
 
  getCommands();//Pulls current available radio commands
  
  printRadioData(); //打印RC的pwm值 1000-2000；

  loopRate(2000);
 
}

void getCommands() 
{
   
    sbus.read(&sbusChannels[0],&sbusFailSafe,&sbusLostFrame);
    
      //sBus scaling below is for Taranis-Plus and X4R-SB
      float scale = 0.615;  
      float bias  = 895.0; 
      channel_1_pwm = sbusChannels[0] * scale + bias;
      channel_2_pwm = sbusChannels[1] * scale + bias;
      channel_3_pwm = sbusChannels[2] * scale + bias;
      channel_4_pwm = sbusChannels[3] * scale + bias;
      channel_5_pwm = sbusChannels[4] * scale + bias;
      channel_6_pwm = sbusChannels[5] * scale + bias; 
           
}

  void printRadioData() 
{
  if (current_time - print_counter > 10000) 
  {
    print_counter = micros();
    Serial.print(F(" CH1: "));
    Serial.print(channel_1_pwm);
    Serial.print(F(" CH2: "));
    Serial.print(channel_2_pwm);
    Serial.print(F(" CH3: "));
    Serial.print(channel_3_pwm);
    Serial.print(F(" CH4: "));
    Serial.print(channel_4_pwm);
    Serial.print(F(" CH5: "));
    Serial.print(channel_5_pwm);
    Serial.print(F(" CH6: "));
    Serial.println(channel_6_pwm);
    //delay(1000);
  }
}

//控制循环频率
  void loopRate(int freq) 
{

  float invFreq = 1.0/freq*1000000.0;
  unsigned long checker = micros();
  
  //Sit in loop until appropriate time has passed
  while (invFreq > (checker - current_time))
   {
    checker = micros();
   }
}


//打印dt
  void printLoopRate()
{
    if (current_time - print_counter > 10000)
    {
    print_counter = micros();
    Serial.print(F("dt = "));
    Serial.println(dt*1000000.0);
    }
}




