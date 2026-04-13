#include <Arduino.h>
#include <MKRWAN.h>

LoRaModem modem;

// valori generati dal Built-in Network Server del gateway
String appEui = "0000000000000000"; // App EUI (16 caratteri)
String appKey = "8248d771da1b2ae23def6d7d7526e35c"; // App Key (32 caratteri)

void setup() {
  Serial.begin(115200);
  while (!Serial); // Attende l'apertura del Monitor Seriale

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
  Serial.println("-----------------------------------");
  Serial.println("Invio pacchetto (Uplink) in corso...");
  
  modem.beginPacket();
  modem.print("Ciao Gateway!");
  
  // timestamp
  Serial.print("Timestamp invio [s]: ");
  Serial.println(millis()/1000);

  // Invia il pacchetto richiedendo una conferma (ACK)
  int err = modem.endPacket(true);
  
  if (err > 0) {
    Serial.println("Conferma (ACK) ricevuta dal server!");

    delay(500); // Piccola pausa prima di leggere eventuali messaggi in arrivo
    
    // --- NUOVA SEZIONE: LETTURA DELLA RISPOSTA (DOWNLINK) ---
    // Controlliamo se il gateway ha approfittato della conferma 
    // per inviarci dei dati o dei comandi.
    if (modem.available()) {
      Serial.print("Il gateway ha allegato un messaggio: ");
      String rispostaGateway = "";
      
      // Legge tutti i byte in arrivo
      while (modem.available()) {
        rispostaGateway += (char)modem.read();
      }
      
      Serial.println(rispostaGateway);
      
      // Qui l'Arduino "risponde" al comando eseguendo un'azione locale
      // Esempio: se dal gateway invii il testo "ON"
      if (rispostaGateway == "ON") {
         Serial.println("Azione: Accendo un rele/LED!");
         // digitalWrite(LED_BUILTIN, HIGH);
      }
      
    } else {
      Serial.println("Nessun messaggio testuale dal gateway (solo l'ACK di conferma).");
    }
    // --------------------------------------------------------

  } else {
    Serial.println("Errore di invio o nessuna conferma (ACK) ricevuta in tempo.");
  }

  Serial.println("Attesa di 30 secondi...");
  delay(30000); 
}