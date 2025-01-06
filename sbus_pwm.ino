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
#include<Servo.h>
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

int a = 0;//全局变量 作为开关使用

float dt;
unsigned long current_time, prev_time;
unsigned long print_counter, serial_counter;

unsigned long channel_1_pwm, channel_2_pwm, channel_3_pwm, channel_4_pwm, channel_5_pwm,channel_6_pwm;// channel_7_pwm;
unsigned long channel_1_pwm_prev, channel_2_pwm_prev, channel_3_pwm_prev, channel_4_pwm_prev;





//****************************************
//准备工作做好， 各个功能端口打开
//****************************************

//定义4个对象分别伺服4个电机
Servo myServo1;
Servo myServo2;
Servo myServo3;
Servo myServo4;

int comint=0;
int comint1=0;
int comint2=0;
int comint3=0;
int comint4=0;

String comStr = "";


void setup() 
{

  Serial.begin(9600);
  delay(100);
 
  //绑定电机引脚号
  myServo1.attach(0);//keeli 1.01版 引脚
  myServo1.writeMicroseconds(1000);
  myServo2.attach(1);
  myServo2.writeMicroseconds(1000);
  myServo3.attach(11);
  myServo3.writeMicroseconds(1000);
  myServo4.attach(9);
  myServo4.writeMicroseconds(1000);

 
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
}


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
   Serial.println("----please input your password-----");
   while(!(Serial.parseInt()==1));//软件锁（阻塞）防止运行直接跑起来   
   a=1;
   Serial.println("key ok, you can proceed... ");
  }
 
 // getCommands();//读取无线命令信号（sbus）
  
  //printRadioData(); //打印RC的pwm值 1000-2000；
   
 // pwm_commandMotors();//向电机发送命令脉冲

  Serial_input();//串口输入pwm并发送命令脉冲给电机

  loopRate(2000);
 
}
//————————————————————————————————自定义函数————————————————————————————————
void Serial_input()
{
  if(Serial.available()>0)
  {
    delay(10);
    comStr = Serial.readString();
    comint = comStr.toInt();
     comint1= comint;
     comint2= comint;
     comint3= comint;
     comint4= comint;

    comint1 = constrain(comint1,1000,2000);
    comint2 = constrain(comint2,1000,2000);
    comint3 = constrain(comint3,1000,2000);
    comint4 = constrain(comint4,1000,2000);
    myServo1.writeMicroseconds(comint1);
    myServo2.writeMicroseconds(comint2);
    myServo3.writeMicroseconds(comint3);
    myServo4.writeMicroseconds(comint4);

    Serial.println(comint1);
    Serial.println(comint2);
    Serial.println(comint3);
    Serial.println(comint4);
  }

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

void pwm_commandMotors()
{
  comint1=channel_1_pwm;
  comint1=constrain(comint1,1000,2000);
  myServo1.writeMicroseconds(comint1);

  comint2=channel_2_pwm;
  comint2=constrain(comint2,1000,2000);
  myServo2.writeMicroseconds(comint2);

  comint3 = channel_3_pwm;
  comint3 = constrain(comint3,1000,2000);   
  myServo3.writeMicroseconds(comint3);

  comint4 =channel_4_pwm;
  comint4 = constrain(comint4,1000,2000);   
  myServo4.writeMicroseconds(comint4);

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




