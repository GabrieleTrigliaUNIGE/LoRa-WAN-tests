#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ==============================================================
// CONFIGURAZIONI HARDWARE E PIN
// ==============================================================

// Pin I2C specifici per Heltec V3
#define I2C_SDA_PIN 17
#define I2C_SCL_PIN 18

// ==============================================================
// CONFIGURAZIONI LOGICHE
// ==============================================================

// Pausa tra l'invio di un pacchetto e l'altro (in millisecondi)
#define APP_TX_DUTYCYCLE 15000

#endif // CONFIG_H