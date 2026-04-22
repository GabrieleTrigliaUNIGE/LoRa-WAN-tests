#include <Arduino.h>
#include <MKRWAN.h>

LoRaModem modem;

// Le tue chiavi OTAA
String appEui = ""; 
String appKey = "8248d771da1b2ae23def6d7d7526e35c"; 

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Avvio modulo LoRaWAN (Modalita' Ricevitore)...");
  
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
  
  Serial.println("Connesso! In attesa di messaggi dal Gateway...");
  modem.setADR(true);
}

void loop() {
  // 1. Inviamo un pacchetto "esca" piccolissimo solo per aprire le finestre di ricezione.
  // Mandiamo un singolo byte (es. il carattere di un punto) per consumare pochissima banda.
  modem.beginPacket();
  modem.print("."); 
  
  // Chiediamo la conferma (true) così il gateway ci risponderà sicuramente
  int err = modem.endPacket(true);

  if (err > 0) {
    // 2. Il pacchetto è andato. Ora aspettiamo mezzo secondo che il buffer si riempia
    delay(500); 
    
    // 3. Controlliamo se il gateway ci ha inviato un messaggio (Downlink)
    if (modem.available()) {
      Serial.println("\n>>> NUOVO MESSAGGIO RICEVUTO! <<<");
      Serial.print("Payload (in HEX): ");
      
      // Legge e stampa tutti i byte in arrivo uno per uno
      while (modem.available()) {
        byte byteRicevuto = modem.read();
        
        // Se il numero è minore di 16 (cioè da 0 a F), aggiungiamo uno '0' 
        // davanti per leggerlo meglio (es. "01" invece di "1")
        if (byteRicevuto < 0x10) {
          Serial.print("0");
        }
        
        // Stampiamo il byte in formato Esadecimale (HEX)
        Serial.print(byteRicevuto, HEX);
        // Aggiungiamo uno spazio per separare i numeri
        Serial.print(" "); 
      }
      
      Serial.println(); // Andiamo a capo alla fine del messaggio
      Serial.println("-----------------------------------");
      
    } else {
      // Se non c'è niente, stampiamo solo un puntino per far capire che sta lavorando
      Serial.print("."); 
    }

  } else {
    Serial.println("Errore di rete. Gateway non raggiungibile.");
  }

  // 4. Pausa obbligatoria per legge (Duty Cycle).
  // Mettendo 60 secondi, avrai un ritardo massimo di 1 minuto per ricevere i comandi.
  delay(60000); 
}