#include <Arduino.h>
#include <MKRWAN.h>

LoRaModem modem;

// Le tue chiavi OTAA
String appEui = ""; 
String appKey = "8248d771da1b2ae23def6d7d7526e35c"; 

// ---> CAMBIA QUESTO NUMERO PER I TUOI TEST <---
const int DIMENSIONE_PAYLOAD = 60; 

byte pacchettoDaInviare[250];

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Avvio modulo LoRaWAN (Modalita' Trasmettitore)...");
  
  if (!modem.begin(EU868)) {
    Serial.println("Errore modulo radio!");
    while (1) {} 
  };

  Serial.println("Connessione OTAA in corso...");
  int connected = modem.joinOTAA(appEui, appKey);
  
  if (!connected) {
    Serial.println("Connessione fallita.");
    while (1) {} 
  }
  
  Serial.println("Connesso!");
  modem.setADR(true); // Attiviamo l'Adaptive Data Rate

  // Prepariamo il pacchetto pesante (riempie l'array con 1, 2, 3, 4...)
  for (int i = 0; i < DIMENSIONE_PAYLOAD; i++) {
    pacchettoDaInviare[i] = i + 1;
  }
}

void loop() {
  Serial.print("\nSto provando a inviare ");
  Serial.print(DIMENSIONE_PAYLOAD);
  Serial.println(" Byte...");

  modem.beginPacket();
  modem.write(pacchettoDaInviare, DIMENSIONE_PAYLOAD); 
  
  // Usiamo 'true' per chiedere la conferma di ricezione al gateway
  int err = modem.endPacket(true);

  if (err > 0) {
    Serial.println(">>> SUCCESSO! Il Gateway ha ricevuto il pacchetto! <<<");
  } else {
    Serial.println("ERRORE. Pacchetto respinto (o per limite hardware o per Duty Cycle).");
  }

  // ATTENZIONE AL DUTY CYCLE!
  // Se invii 222 byte, il tempo in aria è lunghissimo. 
  // Dobbiamo far riposare la scheda per ALMENO 2 minuti per non bloccare il Murata.
  Serial.println("Pausa di raffreddamento Duty Cycle (2 minuti)...");
  delay(120000); 
}
