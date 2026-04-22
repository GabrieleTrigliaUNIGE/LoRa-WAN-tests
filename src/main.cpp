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
  // Inviamo un pacchetto "esca" piccolissimo solo per aprire le finestre di ricezione.
  // Mandiamo un singolo byte (es. il carattere di un punto) per consumare pochissima banda.
  modem.beginPacket();
  modem.print("."); 
  
  // Chiediamo la conferma (true) così il gateway ci risponderà sicuramente
  int err = modem.endPacket(true);

  if (err > 0) {
    
    int portaRicevuta = modem.getDownlinkPort();

    // 3. Ora controlliamo se ci sono dati
    if (modem.available()) {
      Serial.println("\n>>> NUOVO MESSAGGIO RICEVUTO! <<<");
      Serial.print("Ricevuto sulla FPort: ");
      Serial.println(portaRicevuta);
      Serial.print("Payload (HEX): ");
      
      while (modem.available()) {
        byte byteRicevuto = modem.read();
        if (byteRicevuto < 0x10) { Serial.print("0"); }
        Serial.print(byteRicevuto, HEX);
        Serial.print(" "); 
      }
      
      Serial.println("\n-----------------------------------");
    } else {
      Serial.print("."); // Indica che l'Uplink è andato a buon fine ma non c'erano allegati
    }
  } else {
    Serial.println("\n\tErrore: Pacchetto non inviato (Duty Cycle o rete assente)\n");
  }


  // 4. Pausa obbligatoria per legge (Duty Cycle).
  delay(60000); 
}