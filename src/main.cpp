#include <MKRWAN.h>

LoRaModem modem;

// --- INSERISCI QUI LE CHIAVI PRESE DAL RAK ---
String appEui = "a8cb25c9bcd6ea9a"; 
String appKey = "e3839f88390a896337a3bd5cd0ba96c8"; 

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Avvio modulo MKR WAN 1310...");
  if (!modem.begin(EU868)) {
    Serial.println("Errore modulo radio!");
    while (1) {}
  };

  Serial.println("Connessione OTAA al Gateway RAK in corso...");
  if (!modem.joinOTAA(appEui, appKey)) {
    Serial.println("Connessione fallita. Controlla le chiavi!");
    while (1) {}
  }
  
  Serial.println("✅ Connesso alla rete!");
  modem.setADR(true); // Lasciamo decidere la velocità alla rete
}

void loop() {
  // Simuliamo 6 Byte di dati da un accelerometro (Asse X, Y, Z)
  // Mettiamo dei valori a caso ma riconoscibili
  byte payloadSimulato[6];
  payloadSimulato[0] = 0x00; // X alto
  payloadSimulato[1] = 0x0A; // X basso (Valore 10)
  payloadSimulato[2] = 0x00; // Y alto
  payloadSimulato[3] = 0x14; // Y basso (Valore 20)
  payloadSimulato[4] = 0x00; // Z alto
  payloadSimulato[5] = 0x1E; // Z basso (Valore 30)

  Serial.println("Invio pacchetto finto accelerometro (6 Byte)...");

  modem.beginPacket();
  modem.write(payloadSimulato, 6); 
  int err = modem.endPacket(false); // Falso = Unconfirmed (risparmia batteria e rete)

  if (err > 0) {
    Serial.println(">>> Dati inviati con successo! <<<");
  } else {
    Serial.println("Errore durante l'invio.");
  }

  // Pausa di 30 secondi (abbondante per il Duty Cycle visto che mandiamo solo 6 byte)
  delay(30000); 
}
