#include <Arduino.h>
#include <MKRWAN.h>
#include <CayenneLPP.h> // 1. Includi la libreria

LoRaModem modem;
CayenneLPP lpp(51); // 2. Crea l'oggetto LPP (51 byte è la dimensione massima sicura)

String appEui = "0cb18b9ee0d81424"; // Inserisci il tuo App EUI (16 caratteri)
String appKey = "5b1762235cb448bf5f3d5b2a3b029426"; // Inserisci la tua App Key (32 caratteri)

void setup() {
  // Inizializza la seriale per leggere i log sul computer
  Serial.begin(115200);
  while (!Serial); // Attende l'apertura del Monitor Seriale

  Serial.println("Avvio modulo LoRaWAN del MKR WAN 1310...");
  
  // Imposta la frequenza per l'Europa (EU868)
  if (!modem.begin(EU868)) {
    Serial.println("Errore: impossibile avviare il modulo radio!");
    while (1) {} // Ferma il programma
  };

  // Stampa il DevEUI. Copialo e inseriscilo nel gateway quando crei il dispositivo!
  Serial.print("Il DevEUI della tua scheda è: ");
  Serial.println(modem.deviceEUI());

  Serial.println("Tentativo di connessione al Gateway (Join OTAA)...");
  
  // Avvia la connessione usando le chiavi
  int connected = modem.joinOTAA(appEui, appKey);
  
  if (!connected) {
    Serial.println("Connessione fallita. Controlla che le chiavi siano identiche sul Gateway.");
    while (1) {} // Ferma il programma
  }
  
  Serial.println("Connessione al gateway riuscita con successo!");
  
  // Imposta la velocità di trasmissione adattiva (ADR)
  modem.setADR(true);
}

void loop() {
  // Simula la lettura di due sensori
  float miaTemperatura = 25.4;
  float miaUmidita = 60.0;

  // 3. Pulisci il pacchetto precedente
  lpp.reset();

  // 4. Aggiungi i dati al pacchetto Cayenne LPP
  // Formato: add[TipoDato](Canale, Valore)
  lpp.addTemperature(1, miaTemperatura);    // Canale 1: Temperatura
  lpp.addRelativeHumidity(2, miaUmidita);   // Canale 2: Umidità

  // 5. Invia il pacchetto via LoRaWAN
  Serial.println("Invio dati...");
  modem.beginPacket();
  modem.write(lpp.getBuffer(), lpp.getSize()); // Prende i byte formattati e li spedisce
  int err = modem.endPacket(true);

  if (err > 0) {
    Serial.println("Messaggio inviato correttamente!");
  } else {
    Serial.println("Errore nell'invio.");
  }

  delay(60000); // Aspetta 1 minuto prima del prossimo invio
}
