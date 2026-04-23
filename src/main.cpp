#include <Arduino.h>
#include <MKRWAN.h>

LoRaModem modem;

String appEui = ""; 
String appKey = "8248d771da1b2ae23def6d7d7526e35c"; 

byte pacchettoDaInviare[250];
int contatorePacchetti = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Avvio modulo LoRaWAN...");
  if (!modem.begin(EU868)) { while (1) {} };

  Serial.println("Connessione OTAA in corso...");
  if (!modem.joinOTAA(appEui, appKey)) { while (1) {} }
  
  Serial.println("Connesso!");
  
  // 1. Lasciamo l'ADR ACCESO. Facciamo decidere alla rete.
  modem.setADR(true); 
}

void loop() {
  contatorePacchetti++;
  
  
  // Per i primi 10 pacchetti inviamo solo 10 byte (il "rodaggio").
  // Dal 11esimo pacchetto in poi, passiamo a 100 byte!
  int payloadAttuale = 50; 
  if (contatorePacchetti > 7) {
    payloadAttuale = 64;
  }

  Serial.print("\nPacchetto n. ");
  Serial.print(contatorePacchetti);
  Serial.print(" | Invio ");
  Serial.print(payloadAttuale);
  Serial.println(" Byte...");

  // Riempiamo il pacchetto (es. 1, 2, 3...)
  for (int i = 0; i < payloadAttuale; i++) {
    pacchettoDaInviare[i] = i + 1;
  }

  modem.beginPacket();
  modem.write(pacchettoDaInviare, payloadAttuale); 
  
  // Usiamo 'true' per la conferma
  int err = modem.endPacket(true);

  if (err > 0) {
    Serial.println(">>> TRASMESSO! <<<");
  } else {
    Serial.println("ERRORE. Rifiutato.");
  }

  // Pausa di 30 secondi. Sufficiente per 10 byte a SF12 senza violare il Duty Cycle
  Serial.println("Attesa di 30 secondi...");
  delay(30000); 
}

