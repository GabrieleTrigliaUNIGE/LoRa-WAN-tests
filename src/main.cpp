#include <Arduino.h>
#include <MKRWAN.h>
#include <CayenneLPP.h>

LoRaModem modem;
CayenneLPP lpp(51);

String appEui = "0cb18b9ee0d81424"; // App EUI (16 caratteri)
String appKey = "5b1762235cb448bf5f3d5b2a3b029426"; // App Key (32 caratteri)

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Avvio modulo LoRaWAN del MKR WAN 1310...");
  
  // Imposta la frequenza per l'Europa (EU868)
  if (!modem.begin(EU868)) {
    Serial.println("Errore: impossibile avviare il modulo radio!");
    while (1) {} 
  };

  Serial.print("Il DevEUI della tua scheda è: ");
  Serial.println(modem.deviceEUI());

  Serial.println("Tentativo di connessione al Gateway (Join OTAA)...");
  
  int connected = modem.joinOTAA(appEui, appKey);
  
  if (!connected) {
    Serial.println("Connessione fallita. Controlla che le chiavi siano identiche sul Gateway.");
    while (1) {}
  }
  
  Serial.println("Connessione al gateway riuscita con successo!");
  
  // Imposta la velocità di trasmissione adattiva (ADR)
  modem.setADR(true);
}

void loop() {
  // Simula la lettura di due sensori
  float miaTemperatura = 25.4;
  float miaUmidita = 60.0;

  lpp.reset();

  // Formato: add[TipoDato](Canale, Valore)
  lpp.addTemperature(1, miaTemperatura);
  lpp.addRelativeHumidity(2, miaUmidita);

  // Invia il pacchetto via LoRaWAN
  Serial.println("Invio dati...");
  modem.beginPacket();
  modem.write(lpp.getBuffer(), lpp.getSize()); // Prende i byte formattati e li spedisce
  int err = modem.endPacket(true);

  if (err > 0) {
    Serial.println("Messaggio inviato correttamente!");
  } else {
    Serial.println("Errore nell'invio.");
  }

  delay(60000);
}
