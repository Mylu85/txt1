//Author:Mr lu
//Project Start:2025.01.02
//Last updated:2025.01.02

//注意：
//teensy4+keeli+mpu6050
//mpu6050默认接I2C 0

#include"src/MPU6050/MPU6050.h"
#include<Wire.h>

#define USE_MPU6050_I2C
MPU6050 mpu6050;


//------------------------------------------
#define GYRO_250DPS //Default
#define ACCEL_2G //Default

//#if defined USE_MPU6050_I2C
  #define GYRO_FS_SEL_250    MPU6050_GYRO_FS_250
  #define GYRO_FS_SEL_500    MPU6050_GYRO_FS_500
  #define GYRO_FS_SEL_1000   MPU6050_GYRO_FS_1000
  #define GYRO_FS_SEL_2000   MPU6050_GYRO_FS_2000
  #define ACCEL_FS_SEL_2     MPU6050_ACCEL_FS_2
  #define ACCEL_FS_SEL_4     MPU6050_ACCEL_FS_4
  #define ACCEL_FS_SEL_8     MPU6050_ACCEL_FS_8
  #define ACCEL_FS_SEL_16    MPU6050_ACCEL_FS_16
//#endif

#if defined GYRO_250DPS
  #define GYRO_SCALE GYRO_FS_SEL_250
  #define GYRO_SCALE_FACTOR 131.0
#elif defined GYRO_500DPS
  #define GYRO_SCALE GYRO_FS_SEL_500
  #define GYRO_SCALE_FACTOR 65.5
#elif defined GYRO_1000DPS
  #define GYRO_SCALE GYRO_FS_SEL_1000
  #define GYRO_SCALE_FACTOR 32.8
#elif defined GYRO_2000DPS
  #define GYRO_SCALE GYRO_FS_SEL_2000
  #define GYRO_SCALE_FACTOR 16.4
#endif

#if defined ACCEL_2G
  #define ACCEL_SCALE ACCEL_FS_SEL_2
  #define ACCEL_SCALE_FACTOR 16384.0
#elif defined ACCEL_4G
  #define ACCEL_SCALE ACCEL_FS_SEL_4
  #define ACCEL_SCALE_FACTOR 8192.0
#elif defined ACCEL_8G
  #define ACCEL_SCALE ACCEL_FS_SEL_8
  #define ACCEL_SCALE_FACTOR 4096.0
#elif defined ACCEL_16G
  #define ACCEL_SCALE ACCEL_FS_SEL_16
  #define ACCEL_SCALE_FACTOR 2048.0
#endif
//-----------------------------------------

//全局变量
float AccX, AccY, AccZ;
float GyroX, GyroY, GyroZ;
float AccX_prev, AccY_prev, AccZ_prev;
float GyroX_prev, GyroY_prev, GyroZ_prev;

float roll_IMU,pitch_IMU,yaw_IMU;

unsigned long current_time,prev_time;
unsigned long print_counter;

int key = 0;
float dt;

//IMU- 每台飞机/陀螺仪需要校准后填在这里。具体参考校准函数。
float AccErrorX = 0.01;
float AccErrorY = 0.03;
float AccErrorZ = 0.06;
float GyroErrorX = -0.98;//已修订x轴角速度
float GyroErrorY = -0.65;//已修订y轴角速度
float GyroErrorZ = -1.77;//已修订z轴角速度


//Filter parameters -
float B_madgwick = 0.04;  //Madgwick filter parameter
float B_accel = 0.14;     //Accelerometer LP filter paramter, (MPU6050 default: 0.14. MPU9250 default: 0.2)
float B_gyro = 0.1;       //Gyro LP filter paramter, (MPU6050 default: 0.1. MPU9250 default: 0.17)
float B_mag = 1.0;        //Magnetometer LP filter parameter

void setup() 
{  

    Serial.begin(500000); 
    delay(100);

    Serial.println();
    Serial.println("------quadro copter------");
    delay(100);
    Serial.println();

    //初始化IMU
    IMUinit();

    delay(5);

}

void loop()
{ 
  //记录当前时间与循环一次所用的时间差
   prev_time = current_time;      
  current_time = micros();      
  dt = (current_time - prev_time)/1000000.0;

 //软件锁
  while(key==0)
  {
    delay(100);
    Serial.println("---- please input your password ----");
    while(!(Serial.parseInt()==1));//等待串口输入正确值（阻塞）
    key = 1;
    Serial.println("-----system online-----");
    delay(100);
  }

  getIMUdata();//获取处理好的数据

 printGyroData();//打印 x,y,z角速度，-250到250度/秒
 //printAccelData(); //打印 x，y，z 轴的加速度（可以近似理解为倾斜程度）(expected: ~ -2 to 2; x,y 0 when level, z 1 when level)

}




void IMUinit() 
{
  
    Wire.begin();
    Wire.setClock(1000000); //datasheet is 400 kHz max
    
    mpu6050.initialize();
    
    if (mpu6050.testConnection() == false) {
      Serial.println("MPU6050 initialization unsuccessful");
      Serial.println("Check MPU6050 wiring or try cycling power");
      while(1) {}
    }

    //From the reset state all registers should be 0x00, so we should be at
    //max sample rate with digital low pass filter(s) off.  All we need to
    //do is set the desired fullscale ranges
    mpu6050.setFullScaleGyroRange(GYRO_SCALE);
    mpu6050.setFullScaleAccelRange(ACCEL_SCALE);
    Serial.println("***MPU6050 selected***");
}


void getIMUdata() 
{
  int16_t AcX,AcY,AcZ,GyX,GyY,GyZ;
  mpu6050.getMotion6(&AcX, &AcY, &AcZ, &GyX, &GyY, &GyZ);

 //Accelerometer(加速度计)
  AccX = AcX / ACCEL_SCALE_FACTOR; 
  AccY = AcY / ACCEL_SCALE_FACTOR;
  AccZ = AcZ / ACCEL_SCALE_FACTOR;
  //Correct the outputs with the calculated error values(用计算的误差值修正输出)
  AccX = AccX - AccErrorX;
  AccY = AccY - AccErrorY;
  AccZ = AccZ - AccErrorZ;
  //LP filter accelerometer data(LP滤波加速度计数据)
  AccX = (1.0 - B_accel)*AccX_prev + B_accel*AccX;
  AccY = (1.0 - B_accel)*AccY_prev + B_accel*AccY;
  AccZ = (1.0 - B_accel)*AccZ_prev + B_accel*AccZ;
  AccX_prev = AccX;
  AccY_prev = AccY;
  AccZ_prev = AccZ;
  //Gyro
  GyroX = GyX / GYRO_SCALE_FACTOR; 
  GyroY = GyY / GYRO_SCALE_FACTOR;
  GyroZ = GyZ / GYRO_SCALE_FACTOR;
  //Correct the outputs with the calculated error values
  GyroX = GyroX - GyroErrorX;
  GyroY = GyroY - GyroErrorY;
  GyroZ = GyroZ - GyroErrorZ;
  //LP filter gyro data
  GyroX = (1.0 - B_gyro)*GyroX_prev + B_gyro*GyroX;
  GyroY = (1.0 - B_gyro)*GyroY_prev + B_gyro*GyroY;
  GyroZ = (1.0 - B_gyro)*GyroZ_prev + B_gyro*GyroZ;
  GyroX_prev = GyroX;
  GyroY_prev = GyroY;
  GyroZ_prev = GyroZ;
 }

 void printGyroData() 
{
  if (current_time - print_counter > 10000) 
  {
    print_counter = micros();
    Serial.print(F("GyroX: "));
    Serial.print(GyroX);
    Serial.print(F(" GyroY: "));
    Serial.print(GyroY);
    Serial.print(F(" GyroZ: "));
    Serial.println(GyroZ);
  }
}

void printAccelData()
{
  if (current_time - print_counter > 10000) 
  {
    print_counter = micros();
    Serial.print(F("AccX: "));
    Serial.print(AccX);
    Serial.print(F(" AccY: "));
    Serial.print(AccY);
    Serial.print(F(" AccZ: "));
    Serial.println(AccZ);
  }
}
