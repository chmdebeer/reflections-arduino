#include <Arduino.h>
#include <N2kTypes.h>
#include "BoatData.h"
#include "MPU9250.h"

// #define CALIBRATE

// an MPU9250 object with the MPU-9250 sensor on I2C bus 0 with address 0x68
MPU9250 IMU(Wire,0x68);

int status;

void setupAttitude(){
  status = IMU.begin();

  if (status < 0) {
    Serial.println("IMU initialization unsuccessful");
    Serial.println("Check IMU wiring or try cycling power");
    Serial.print("Status: ");
    Serial.println(status);
    return;
  }

#ifndef CALIBRATE
  IMU.setGyroBiasX_rads(0.01);
  IMU.setGyroBiasY_rads(0.05);
  IMU.setGyroBiasZ_rads(-0.02);
  IMU.setAccelCalX(0.0, 1.0);
  IMU.setAccelCalY(0.0, 1.0);
  IMU.setAccelCalZ(0.0, 1.0);
  IMU.setMagCalX(27.89, 1.20);
  IMU.setMagCalY(1.92, 1.38);
  IMU.setMagCalZ(-51.94, 0.69);
#endif

#ifdef CALIBRATE
  Serial.println("Calibrating Accell");
  status = IMU.calibrateAccel();
  if (status < 0) {
    Serial.println("Accel Calibration failed");
    Serial.print("Status: ");
    Serial.println(status);
    while(1) {}
  }

  Serial.println("Calibrating Mag");
  delay(5000);
  status = IMU.calibrateMag();
  if (status < 0) {
    Serial.println("Mag Calibration failed");
    Serial.print("Status: ");
    Serial.println(status);
    while(1) {}
  }
#endif

  // setting the accelerometer full scale range to +/-8G
  IMU.setAccelRange(MPU9250::ACCEL_RANGE_2G);
  // setting the gyroscope full scale range to +/-500 deg/s
  IMU.setGyroRange(MPU9250::GYRO_RANGE_250DPS);
  // setting DLPF bandwidth to 20 Hz
  IMU.setDlpfBandwidth(MPU9250::DLPF_BANDWIDTH_5HZ);
  // setting SRD to 19 for a 50 Hz update rate
  IMU.setSrd(99);
  IMU.disableDataReadyInterrupt();

#ifdef CALIBRATE
  Serial.print("Gyro Bias X ");
  Serial.println(IMU.getGyroBiasX_rads());
  Serial.print("Gyro Bias Y ");
  Serial.println(IMU.getGyroBiasY_rads());
  Serial.print("Gyro Bias Z ");
  Serial.println(IMU.getGyroBiasZ_rads());

  Serial.print("Accel Bias X ");
  Serial.print(IMU.getAccelBiasX_mss());
  Serial.print("m/s/s, scale ");
  Serial.println(IMU.getAccelScaleFactorX());

  Serial.print("Accel Bias Y ");
  Serial.print(IMU.getAccelBiasY_mss());
  Serial.print("m/s/s, scale ");
  Serial.println(IMU.getAccelScaleFactorY());

  Serial.print("Accel Bias Z ");
  Serial.print(IMU.getAccelBiasZ_mss());
  Serial.print("m/s/s, scale ");
  Serial.println(IMU.getAccelScaleFactorZ());

  Serial.print("Mag Bias X ");
  Serial.print(IMU.getMagBiasX_uT());
  Serial.print(", scale ");
  Serial.println(IMU.getMagScaleFactorX());

  Serial.print("Mag Bias Y ");
  Serial.print(IMU.getMagBiasY_uT());
  Serial.print(", scale ");
  Serial.println(IMU.getMagScaleFactorY());

  Serial.print("Mag Bias Z ");
  Serial.print(IMU.getMagBiasZ_uT());
  Serial.print(", scale ");
  Serial.println(IMU.getMagScaleFactorZ());

  delay(60000);
#endif
}

void loopAttitude(BoatData &boatData) {
  if (status < 0) {
    return;
  }
  IMU.readSensor();
  boatData.attitude.roll = IMU.getRoll();
  boatData.attitude.pitch = IMU.getPitch();
  boatData.attitude.yaw = IMU.getYaw();

  #ifdef CALIBRATE
  Serial.print("aX=");
  Serial.print(IMU.getAccelX_mss(),4);
  Serial.print("\taY=");
  Serial.print(IMU.getAccelY_mss(),4);
  Serial.print("\taZ=");
  Serial.print(IMU.getAccelZ_mss(),4);
  Serial.print("\tgX=");
  Serial.print(IMU.getGyroX_rads(),4);
  Serial.print("\tgY=");
  Serial.print(IMU.getGyroY_rads(),4);
  Serial.print("\tgZ=");
  Serial.print(IMU.getGyroZ_rads(),4);
  Serial.print("\tmX=");
  Serial.print(IMU.getMagX_uT(),4);
  Serial.print("\tmY=");
  Serial.print(IMU.getMagY_uT(),4);
  Serial.print("\tmZ=");
  Serial.print(IMU.getMagZ_uT(),4);
  Serial.print("\tT=");
  Serial.print(IMU.getTemperature_C(),4);
  Serial.print("\t|\t");

  Serial.print("R=");
  Serial.print(IMU.getRoll(),4);
  Serial.print("\tP=");
  Serial.print(IMU.getPitch(),4);
  Serial.print("\tY=");
  Serial.print(IMU.getYaw(),4);
  Serial.println(" <-");
  delay(100);
#endif
}
