#include "quaternionFilters.h"
#include "MPU9250.h"

MPU9250 IMU_1;
MPU9250 IMU_2;

void setup() {
  Wire.begin();
  Serial.begin(9600);   
  // Nie wiem do końca czy to potrzebne ??
  IMU_1.calibrateMPU9250(IMU_1.gyroBias, IMU_1.accelBias);
  IMU_2.calibrateMPU9250(IMU_2.gyroBias, IMU_2.accelBias);
  IMU_1.initMPU9250();
  IMU_2.initMPU9250();
}

void loop() {
  if (IMU_1.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01) {  
    IMU_1.readAccelData(IMU_1.accelCount);  // Read the x/y/z adc values
    IMU_1.getAres();
    
    // Now we'll calculate the accleration value into actual g's
    // This depends on scale being set
    IMU_1.ax = (float)IMU_1.accelCount[0]*IMU_1.aRes; // - accelBias[0];
    IMU_1.ay = (float)IMU_1.accelCount[1]*IMU_1.aRes; // - accelBias[1];
    IMU_1.az = (float)IMU_1.accelCount[2]*IMU_1.aRes; // - accelBias[2];
    
    IMU_1.readGyroData(IMU_1.gyroCount);  // Read the x/y/z adc values
    IMU_1.getGres();
    
    // Calculate the gyro value into actual degrees per second
    // This depends on scale being set
    IMU_1.gx = (float)IMU_1.gyroCount[0]*IMU_1.gRes;
    IMU_1.gy = (float)IMU_1.gyroCount[1]*IMU_1.gRes;
    IMU_1.gz = (float)IMU_1.gyroCount[2]*IMU_1.gRes;
    
    IMU_1.readMagData(IMU_1.magCount);  // Read the x/y/z adc values
    IMU_1.getMres();
    // User environmental x-axis correction in milliGauss, should be
    // automatically calculated
    IMU_1.magbias[0] = +470.;
    // User environmental x-axis correction in milliGauss TODO axis??
    IMU_1.magbias[1] = +120.;
    // User environmental x-axis correction in milliGauss
    IMU_1.magbias[2] = +125.;
    
    // Calculate the magnetometer values in milliGauss
    // Include factory calibration per data sheet and user environmental
    // corrections
    // Get actual magnetometer value, this depends on scale being set
    IMU_1.mx = (float)IMU_1.magCount[0]*IMU_1.mRes*IMU_1.magCalibration[0] -
             IMU_1.magbias[0];
    IMU_1.my = (float)IMU_1.magCount[1]*IMU_1.mRes*IMU_1.magCalibration[1] -
             IMU_1.magbias[1];
    IMU_1.mz = (float)IMU_1.magCount[2]*IMU_1.mRes*IMU_1.magCalibration[2] -
             IMU_1.magbias[2];
  }

  IMU_1.updateTime();
  MahonyQuaternionUpdate(IMU_1.ax, IMU_1.ay, IMU_1.az, IMU_1.gx*DEG_TO_RAD,
                       IMU_1.gy*DEG_TO_RAD, IMU_1.gz*DEG_TO_RAD, IMU_1.my,
                       IMU_1.mx, IMU_1.mz, IMU_1.deltat);

if (IMU_2.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01) {  
    IMU_2.readAccelData(IMU_2.accelCount);  // Read the x/y/z adc values
    IMU_2.getAres();
    
    // Now we'll calculate the accleration value into actual g's
    // This depends on scale being set
    IMU_2.ax = (float)IMU_2.accelCount[0]*IMU_2.aRes; // - accelBias[0];
    IMU_2.ay = (float)IMU_2.accelCount[1]*IMU_2.aRes; // - accelBias[1];
    IMU_2.az = (float)IMU_2.accelCount[2]*IMU_2.aRes; // - accelBias[2];
    
    IMU_2.readGyroData(IMU_2.gyroCount);  // Read the x/y/z adc values
    IMU_2.getGres();
    
    // Calculate the gyro value into actual degrees per second
    // This depends on scale being set
    IMU_2.gx = (float)IMU_2.gyroCount[0]*IMU_2.gRes;
    IMU_2.gy = (float)IMU_2.gyroCount[1]*IMU_2.gRes;
    IMU_2.gz = (float)IMU_2.gyroCount[2]*IMU_2.gRes;
    
    IMU_2.readMagData(IMU_2.magCount);  // Read the x/y/z adc values
    IMU_2.getMres();
    // User environmental x-axis correction in milliGauss, should be
    // automatically calculated
    IMU_2.magbias[0] = +470.;
    // User environmental x-axis correction in milliGauss TODO axis??
    IMU_2.magbias[1] = +120.;
    // User environmental x-axis correction in milliGauss
    IMU_2.magbias[2] = +125.;
    
    // Calculate the magnetometer values in milliGauss
    // Include factory calibration per data sheet and user environmental
    // corrections
    // Get actual magnetometer value, this depends on scale being set
    IMU_2.mx = (float)IMU_2.magCount[0]*IMU_2.mRes*IMU_2.magCalibration[0] -
             IMU_2.magbias[0];
    IMU_2.my = (float)IMU_2.magCount[1]*IMU_2.mRes*IMU_2.magCalibration[1] -
             IMU_2.magbias[1];
    IMU_2.mz = (float)IMU_2.magCount[2]*IMU_2.mRes*IMU_2.magCalibration[2] -
             IMU_2.magbias[2];
  }

  IMU_2.updateTime();
  MahonyQuaternionUpdate(IMU_2.ax, IMU_2.ay, IMU_2.az, IMU_2.gx*DEG_TO_RAD,
                       IMU_2.gy*DEG_TO_RAD, IMU_2.gz*DEG_TO_RAD, IMU_2.my,
                       IMU_2.mx, IMU_2.mz, IMU_2.deltat);
                       
  
  printData();
  delay(100);
}

void printData() {
  Serial.print("ax = "); Serial.print((int)1000*IMU_1.ax);
  Serial.print(" ay = "); Serial.print((int)1000*IMU_1.ay);
  Serial.print(" az = "); Serial.print((int)1000*IMU_1.az);
  Serial.println(" mg");

  Serial.print("gx = "); Serial.print( IMU_1.gx, 2);
  Serial.print(" gy = "); Serial.print( IMU_1.gy, 2);
  Serial.print(" gz = "); Serial.print( IMU_1.gz, 2);
  Serial.println(" deg/s");

  Serial.print("mx = "); Serial.print( (int)IMU_1.mx );
  Serial.print(" my = "); Serial.print( (int)IMU_1.my );
  Serial.print(" mz = "); Serial.print( (int)IMU_1.mz );
  Serial.println(" mG");

  Serial.print("q0 = "); Serial.print(*getQ());
  Serial.print(" qx = "); Serial.print(*(getQ() + 1));
  Serial.print(" qy = "); Serial.print(*(getQ() + 2));
  Serial.print(" qz = "); Serial.println(*(getQ() + 3));






  Serial.print("ax = "); Serial.print((int)1000*IMU_2.ax);
  Serial.print(" ay = "); Serial.print((int)1000*IMU_2.ay);
  Serial.print(" az = "); Serial.print((int)1000*IMU_2.az);
  Serial.println(" mg");

  Serial.print("gx = "); Serial.print( IMU_2.gx, 2);
  Serial.print(" gy = "); Serial.print( IMU_2.gy, 2);
  Serial.print(" gz = "); Serial.print( IMU_2.gz, 2);
  Serial.println(" deg/s");

  Serial.print("mx = "); Serial.print( (int)IMU_2.mx );
  Serial.print(" my = "); Serial.print( (int)IMU_2.my );
  Serial.print(" mz = "); Serial.print( (int)IMU_2.mz );
  Serial.println(" mG");

  Serial.print("q0 = "); Serial.print(*getQ());
  Serial.print(" qx = "); Serial.print(*(getQ() + 1));
  Serial.print(" qy = "); Serial.print(*(getQ() + 2));
  Serial.print(" qz = "); Serial.println(*(getQ() + 3));
}

