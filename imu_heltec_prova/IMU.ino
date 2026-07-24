#include "IMU_Handler.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// Istanza privata del sensore (non visibile dal file .ino)
Adafruit_MPU6050 mpu;

bool initMPU() {
  // Inizializza il bus I2C usando i pin definiti nel Config.h
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  
  if (!mpu.begin()) {
    return false;
  }

  // Configurazioni ottimali per il pendolo
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ); 

  return true;
}

MPUData readMPUData() {
  sensors_event_t accel, gyro, temp;
  mpu.getEvent(&accel, &gyro, &temp);

  MPUData data;
  // Conversione Fixed-Point: togliamo i decimali (float) moltiplicando per 100.
  // Es. 9.81 m/s^2 diventa 981. Più efficiente da inviare via radio!
  data.accX = accel.acceleration.x * 100;
  data.accY = accel.acceleration.y * 100;
  data.accZ = accel.acceleration.z * 100;

  return data;
}