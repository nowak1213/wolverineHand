#include <Wire.h> // Allows to communicate with I2C devices
#include "MPU9250.h"
#include <SoftI2cMaster.h>
#include "Adafruit_VCNL4010.h"
#include <SoftwareSerial.h>
#include <SerialCommand.h>
#include <Servo.h>
#define Debug false  // Set to true to get all data output for debugging

SerialCommand sCmd;
Servo myservo1;
Servo myservo2;

double a1 = 2217000000000;
double b1 = -0.01119;
double c1 = 49.13;
double d1 = -0.0007755;

double a2 = 489.8;
double b2 = -0.002101;
double c2 = 4.883;
double d2 = -0.0000935;

const int IMU_MAX = 2; // number of IMU to switch between
const int SD0[IMU_MAX] = {7, 8}; // pins for SD0 connection to IMU

MPU9250 con_IMU;

word distance = 0x00;
uint8_t data1 = 0x00;
uint8_t data2 = 0x00;

SoftI2cMaster i2c;

// Digital Pins for second I2C 
#define SCL_PIN 10 
#define SDA_PIN 11

Adafruit_VCNL4010 vcnl;

double accX[IMU_MAX], accY[IMU_MAX], accZ[IMU_MAX];
double gyroX[IMU_MAX], gyroY[IMU_MAX], gyroZ[IMU_MAX];
double magX[IMU_MAX], magY[IMU_MAX], magZ[IMU_MAX];

double roll[IMU_MAX], pitch[IMU_MAX], yaw[IMU_MAX];
double p_roll[IMU_MAX], p_pitch[IMU_MAX], p_yaw[IMU_MAX];
double rollAngle[IMU_MAX], pitchAngle[IMU_MAX], yawAngle[IMU_MAX];

double accXfilt[IMU_MAX], accYfilt[IMU_MAX], accZfilt[IMU_MAX];

uint32_t timer[IMU_MAX];

void setup() {
  delay(100);
  Wire.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  myservo1.attach(3);
  myservo2.attach(5);

  // set all IMU in deactive mode (HIGH) on I2C  
  for (int i = 0; i < IMU_MAX; i++) {
    pinMode(SD0[i], OUTPUT);
    digitalWrite(SD0[i], HIGH);
  }
  
  // set configuration for each IMU
  for (int i = 0; i < IMU_MAX; i++) {
    digitalWrite(SD0[i], LOW);
    if (Debug) {
      byte c = con_IMU.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
      Serial.print("MPU9250 nr "); Serial.print(i); Serial.print(", ID (Should be 0x71): "); Serial.println(c, HEX);
      if (c == 0x71) {
        Serial.println("MPU9250 is online...");
        
        // Start by performing self test and reporting values
        con_IMU.MPU9250SelfTest(con_IMU.SelfTest);
        Serial.print("x-axis self test: acceleration trim within : ");
        Serial.print(con_IMU.SelfTest[0],1); Serial.println("% of factory value");
        Serial.print("y-axis self test: acceleration trim within : ");
        Serial.print(con_IMU.SelfTest[1],1); Serial.println("% of factory value");
        Serial.print("z-axis self test: acceleration trim within : ");
        Serial.print(con_IMU.SelfTest[2],1); Serial.println("% of factory value");
        Serial.print("x-axis self test: gyration trim within : ");
        Serial.print(con_IMU.SelfTest[3],1); Serial.println("% of factory value");
        Serial.print("y-axis self test: gyration trim within : ");
        Serial.print(con_IMU.SelfTest[4],1); Serial.println("% of factory value");
        Serial.print("z-axis self test: gyration trim within : ");
        Serial.print(con_IMU.SelfTest[5],1); Serial.println("% of factory value");
      }
    }
    con_IMU.calibrateMPU9250(con_IMU.gyroBias, con_IMU.accelBias);
    if (Debug) {
      Serial.println("MPU9250 bias");
      Serial.println(" x   y   z  ");
      
      Serial.print((int)(1000*con_IMU.accelBias[0]));
      Serial.print((int)(1000*con_IMU.accelBias[1]));
      Serial.print((int)(1000*con_IMU.accelBias[2]));
      Serial.println(" mg");
      
      Serial.print(con_IMU.gyroBias[0], 1);
      Serial.print(con_IMU.gyroBias[1], 1);
      Serial.print(con_IMU.gyroBias[2], 1);
      Serial.println(" o/s");
    }
    con_IMU.initMPU9250();
    if (Debug) {
      Serial.print("MPU9250 nr "); Serial.print(i); Serial.println(" initialized for active data mode....");
      byte d = con_IMU.readByte(AK8963_ADDRESS, WHO_AM_I_AK8963);
      Serial.print("AK8963 nr "); Serial.print(i); Serial.print(", ID (Should be 0x48): "); Serial.println(d, HEX);
    }
    con_IMU.initAK8963(con_IMU.magCalibration);
    if (Debug) {
      Serial.print("AK8963 nr "); Serial.print(i); Serial.println(" initialized for active data mode....");
      //  Serial.println("Calibration values: ");
      Serial.print("X-Axis sensitivity adjustment value ");
      Serial.println(con_IMU.magCalibration[0], 2);
      Serial.print("Y-Axis sensitivity adjustment value ");
      Serial.println(con_IMU.magCalibration[1], 2);
      Serial.print("Z-Axis sensitivity adjustment value ");
      Serial.println(con_IMU.magCalibration[2], 2);
    }
    digitalWrite(SD0[i], HIGH);
  }

  i2c_transaction(0x83,20);
  i2c_transaction(0x89,3);

  vcnl.begin();

  delay(100);
  for (int i = 0; i < IMU_MAX; i++) {
    digitalWrite(SD0[i], LOW);
    
    updateMPU9250(i);
    updateAngles(i);
    
    digitalWrite(SD0[i], HIGH);
    timer[i] = micros();
  }
  
  Serial.begin(9600);
  sCmd.addCommand("servo1True", servo1TrueHandler);
  sCmd.addCommand("servo1False", servo1FalseHandler);
  sCmd.addCommand("servo2True", servo2TrueHandler);
  sCmd.addCommand("servo2False", servo2FalseHandler);
}

void loop() {
  if (Serial.available() > 0)
    sCmd.readSerial();

  for (int i = 0; i < IMU_MAX; i++) {
    digitalWrite(SD0[i], LOW);
    
    updateMPU9250(i);
    
    double dt = (double)(micros() - timer[i]) / 1000000; // Calculate delta time
    timer[i] = micros();

    updateAngles(i);

    double alfaGyro = 0.98;
    
    roll[i] = alfaGyro * (roll[i] + gyroX[i] * dt) + (1 - alfaGyro) * rollAngle[i];
    pitch[i] = alfaGyro * (pitch[i] + gyroY[i] * dt) + (1 - alfaGyro) * pitchAngle[i];
    yaw[i] = alfaGyro * (yaw[i] + gyroZ[i] * dt) + (1 - alfaGyro) * yawAngle[i];

    double alfaAcc = 0.5;
    
    accXfilt[i] = (1 - alfaAcc) * accXfilt[i] + alfaAcc * accX[i]; 
    accYfilt[i] = (1 - alfaAcc) * accYfilt[i] + alfaAcc * accY[i]; 
    accZfilt[i] = (1 - alfaAcc) * accZfilt[i] + alfaAcc * accZ[i]; 
    
    printIMUData(i);
    
    digitalWrite(SD0[i], HIGH);
  }
  
  i2c_transaction(0x80,8);
  double x;
  double y;
  x = distance;
  y = a2 * exp(b2 * x) + c2 * exp(d2 * x);
  Serial.print(y); Serial.print(",");
  x = vcnl.readProximity();
  y = a1 * exp(b1 * x) + c1 * exp(d1 * x);
  Serial.print(y); Serial.print(",");
  Serial.println("");
  
  delay(100);
}

void printIMUData(int n) {
  // Gyroscope
  Serial.print((p_roll[n] - roll[n]), 2); Serial.print(",");
  Serial.print((p_pitch[n] - pitch[n]), 2); Serial.print(",");
  Serial.print((p_yaw[n] - yaw[n]), 2); Serial.print(",");
  p_roll[n] = roll[n];
  p_pitch[n] = pitch[n];
  p_yaw[n] = yaw[n];
//  Serial.print(accXfilt[n], 2); Serial.print(",");
//  Serial.print(accYfilt[n], 2); Serial.print(",");
//  Serial.print(accZfilt[n], 2); Serial.print(",");
}

void updateMPU9250(int n) {
  if (con_IMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01) {  
    con_IMU.readAccelData(con_IMU.accelCount);  // Read the x/y/z adc values
    con_IMU.getAres();
    
    // Now we'll calculate the accleration value into actual g's
    // This depends on scale being set
    accX[n] = (float)con_IMU.accelCount[0]*con_IMU.aRes; // - accelBias[0];
    accY[n] = (float)con_IMU.accelCount[1]*con_IMU.aRes; // - accelBias[1];
    accZ[n] = (float)con_IMU.accelCount[2]*con_IMU.aRes; // - accelBias[2];
    
    con_IMU.readGyroData(con_IMU.gyroCount);  // Read the x/y/z adc values
    con_IMU.getGres();
    
    // Calculate the gyro value into actual degrees per second
    // This depends on scale being set
    gyroX[n] = (float)con_IMU.gyroCount[0]*con_IMU.gRes;
    gyroY[n] = (float)con_IMU.gyroCount[1]*con_IMU.gRes;
    gyroZ[n] = (float)con_IMU.gyroCount[2]*con_IMU.gRes;

    con_IMU.readMagData(con_IMU.magCount);  // Read the x/y/z adc values
    con_IMU.getMres();
    // User environmental x-axis correction in milliGauss, should be
    // automatically calculated
    con_IMU.magbias[0] = +470.;
    // User environmental x-axis correction in milliGauss TODO axis??
    con_IMU.magbias[1] = +120.;
    // User environmental x-axis correction in milliGauss
    con_IMU.magbias[2] = +125.;
    
    // Calculate the magnetometer values in milliGauss
    // Include factory calibration per data sheet and user environmental
    // corrections
    // Get actual magnetometer value, this depends on scale being set
    magX[n] = (float)con_IMU.magCount[0]*con_IMU.mRes*con_IMU.magCalibration[0] -
               con_IMU.magbias[0];
    magY[n] = (float)con_IMU.magCount[1]*con_IMU.mRes*con_IMU.magCalibration[1] -
               con_IMU.magbias[1];
    magZ[n] = (float)con_IMU.magCount[2]*con_IMU.mRes*con_IMU.magCalibration[2] -
               con_IMU.magbias[2];
  }
  con_IMU.updateTime();
}

void updateAngles(int n) {
  pitchAngle[n] = atan(accX[n]/sqrt(accY[n]*accY[n] + accZ[n]*accZ[n])) * RAD_TO_DEG;
  rollAngle[n] = atan(accY[n]/sqrt(accX[n]*accX[n] + accZ[n]*accZ[n])) * RAD_TO_DEG;
  yawAngle[n] = atan(accZ[n]/sqrt(accX[n]*accX[n] + accZ[n]*accZ[n])) * RAD_TO_DEG;
}

void i2c_transaction(uint8_t reg, uint8_t dat){     
  TwoWireBase *bus;
  bus = &i2c;
  i2c.init(SCL_PIN, SDA_PIN);
  
  bus->start(0x26);
  bus->write(reg);
  bus->write(dat);
  bus->stop();
  
  bus->start(0x26);
  bus->write(0x87);
  bus->stop();
  bus->start(0x27);
  data1 = bus->read(0);
  data2 = bus->read(1);
  bus->stop();
  
  distance = word(data1,data2);
}

void servo1TrueHandler (const char *command) {
  myservo1.write(160);
}

void servo1FalseHandler (const char *command) {
  myservo1.write(90);
}

void servo2TrueHandler (const char *command) {
  myservo2.write(160);
}

void servo2FalseHandler (const char *command) {
  myservo2.write(90);
}

