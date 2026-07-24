#ifndef IMU_HANDLER_H
#define IMU_HANDLER_H

#include <Arduino.h>
#include "Config.h"

// Struttura dati per restituire X, Y e Z contemporaneamente
struct MPUData {
  int16_t accX; // Accelerazione X
  int16_t accY; // Accelerazione Y
  int16_t accZ; // Accelerazione Z
};

// Funzioni pubbliche esposte dal modulo
bool initMPU();
MPUData readMPUData();

#endif // IMU_HANDLER_H