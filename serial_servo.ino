// serial servo 
//Author: Mr lu
//Project Start: 2024.9.13
//last updated:2025.1.2  tested ok

//********************************
//注意：
//波特率需要同步设定为9600

int a = 0;//全局变量 作为开关使用
long pwm = 1000;
int angle = 0;//角度
const int servo1Pin = 9;
const int servo2Pin = 8;

void setup() {

  Serial.begin(9600);
  delay(500);
  Serial.println();
  Serial.println();
  Serial.println(" Serial servo ! ! ! ");
  delay(500);
  Serial.println("hi everyone!");
  delay(100);
  Serial.println(" this time you can control a servo through serial port ! ");
  delay(100);
  Serial.println(" Enjoy the labs and most importantly, HAVE FUN! ");
  Serial.println();
  delay(100);
  Serial.println("enter number 666 to finish the lab");
  delay(1000);
}

void pwmservo(int t){

   for (int i=0;i<3;i++){
    digitalWrite(servo1Pin,HIGH);
    delayMicroseconds(t);
    digitalWrite(servo1Pin,LOW);  
    delay(20);  
   }//end for
  }//end pwmservo

  void pwmservo2(int t){

   for (int i=0;i<3;i++){
    digitalWrite(servo2Pin,HIGH);
    delayMicroseconds(t);
    digitalWrite(servo2Pin,LOW);  
    delay(20);  
   }//end for
  }//end pwmservo

void loop() {

//========= set flag a to 1 enter angle loop
   while(a==0){
   Serial.println("type on keyboard number 1 to set angle。");   
   delay(1000);
   Serial.println("Waiting for key in ...");
   while(!(Serial.parseInt()==1));//经典卡bug定身咒，没有串口输入就一直卡着等。
   //Serial.println("after while");
   a=1;
   Serial.println("a is 1 , you can set up angle from 1-90 ");
   
  }//end while 
  
   
//angle loop , keyin angle number 666 to set flag a off, hence get out while loop     
  while (Serial.available() > 0 &&(a==1)) {
    
    angle=Serial.parseInt();
    
    if (angle>0 && angle<91) { 
      Serial.print("angle set to : ");
      Serial.println(angle);
      pwm=map(angle,0,90,1000,2000);
      Serial.print("pwm is : ");
      Serial.println(pwm);
      pwmservo(pwm);
       pwmservo2(pwm);
      
    }//end if
    a=1;//set flag a back to 1 to keep setting angle until 666
    delay(1000);
     if (angle==666) {
      Serial.println("YOU WIN!!!");
      a=2;//flag out
      }
   }//end while
  
}//end loop
