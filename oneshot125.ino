//oneshot125 
//Author: Mr lu
//Project Start: 2025.1.3
//last updated:2025.1.3

//xxd的电调应该是不支持oneshot125


int key = 0;

const int m1Pin=6;// keeli 左前
const int m2Pin=7;// keeli 右前
const int m3Pin=10;// keeli 右后
const int m4Pin=8;// keeli 左后
int  m1_command_PWM;
int  m2_command_PWM;
int  m3_command_PWM;
int  m4_command_PWM;

void setup() {
  Serial.begin(500000);
  delay(500);
  Serial.println("serial port initiated..");
  
  pinMode(13,OUTPUT);//LED on board
  pinMode(m1Pin,OUTPUT);
  pinMode(m2Pin,OUTPUT);
  pinMode(m3Pin,OUTPUT);
  pinMode(m4Pin,OUTPUT);

  digitalWrite(13,1);

  //初始化所有电机命令125（停止）
  m1_command_PWM = 125;
  m2_command_PWM = 125;
  m3_command_PWM = 125;
  m4_command_PWM = 125;
  
  
  armMotors();
  Serial.println("oneshot125: 125min--250 max");
  delay(1000);
}

void loop() {
  

  
   while(key==0){
   Serial.println("Enter password:");   
   delay(100);
   while(!(Serial.parseInt()==1));//串口阻塞等待（软件锁作用）
  
   key=1;
   m1_command_PWM = 125;
   m2_command_PWM = 125;
   m3_command_PWM = 125;
   m4_command_PWM = 125;

   Serial.println("***motors Online***");
   delay(100);
  }



  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');//读取串口数据
    Serial.print("Input received: ");
    Serial.println(input);
    
    m1_command_PWM = input.toInt();//将接收到的数据转为整数，并设置电机命令值
    m2_command_PWM = input.toInt();
    m3_command_PWM = input.toInt();
    m4_command_PWM = input.toInt();
    
    Serial.println("mx_command_PWM updated: ");
    Serial.println(m1_command_PWM);
    Serial.println(m2_command_PWM);
    Serial.println(m3_command_PWM);
    Serial.println(m4_command_PWM);

  }

    
  commandMotors(); //调用命令电机函数，根据命令值发送给电机

}


//********************************functions******************************
//命令电机函数
void commandMotors() {
  
  int wentLow = 0;
  int pulseStart, timer;
  int flagM1 = 0;
  int flagM2 = 0;
  int flagM3 = 0;
  int flagM4 = 0;

  //Write all motor pins high
  digitalWrite(m1Pin, HIGH);
  digitalWrite(m2Pin, HIGH);
  digitalWrite(m3Pin, HIGH);
  digitalWrite(m4Pin, HIGH);
  
  pulseStart = micros();

  //Write each motor pin low as correct pulse length is reached
  while (wentLow < 4 ) { //Keep going until final (6th) pulse is finished, then done
    timer = micros();
    
    if ((m1_command_PWM <= timer - pulseStart) && (flagM1==0)) {
      digitalWrite(m1Pin, LOW);
      wentLow = wentLow + 1;
      flagM1 = 1;
    }
     if ((m2_command_PWM <= timer - pulseStart) && (flagM2==0)) {
      digitalWrite(m2Pin, LOW);
      wentLow = wentLow + 1;
      flagM2 = 1;
    }
    if ((m3_command_PWM <= timer - pulseStart) && (flagM3==0)) {
      digitalWrite(m3Pin, LOW);
      wentLow = wentLow + 1;
      flagM3 = 1;
    }
    if ((m4_command_PWM <= timer - pulseStart) && (flagM4==0)) {
      digitalWrite(m4Pin, LOW);
      wentLow = wentLow + 1;
      flagM4 = 1;
    }
  
  }
  
}
//电机启动
void armMotors() {
  
  delay(1000);
  Serial.println("arming motors..."); 
  for (int i = 0; i <= 50; i++) {
    digitalWrite(13, HIGH);
    //Serial.println(i); 
    commandMotors();
    delay(2);
    digitalWrite(13, LOW);
    }
    
    Serial.println("done arming motors..."); 
    delay(1000);
}



