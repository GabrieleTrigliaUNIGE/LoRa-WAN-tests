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
      Serial.print("Testo: ");
      
      String messaggioInArrivo = "";
      while (modem.available()) {
        messaggioInArrivo += (char)modem.read();
      }
      
      // Stampiamo il messaggio ricevuto
      Serial.println(messaggioInArrivo);
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
  delay(30000); 
}