#include "LoRaWan_APP.h"

// ======================================================================
// 🔑 CHIAVI LORAWAN (OTAA)
// ======================================================================
uint8_t devEui[] = { 0x22, 0x32, 0x33, 0x00, 0x00, 0x88, 0x88, 0x02 };
uint8_t appEui[] = { 0x50, 0x3c, 0x3f, 0xaf, 0x13, 0x8a, 0xc5, 0xac };
uint8_t appKey[] = { 0x9C, 0x82, 0x92, 0xF2, 0x4E, 0x7C, 0x78, 0xFB, 0xB6, 0x1A, 0xB2, 0xE0, 0x38, 0x4E, 0x43, 0x89 };

/* ABP para (Lasciati vuoti, usiamo OTAA) */
uint8_t nwkSKey[] = { 0x15, 0xb1, 0xd0, 0xef, 0xa4, 0x63, 0xdf, 0xbe, 0x3d, 0x11, 0x18, 0x1e, 0x1e, 0xc7, 0xda,0x85 };
uint8_t appSKey[] = { 0xd7, 0x2c, 0x78, 0x75, 0x8c, 0xdc, 0xca, 0xbf, 0x55, 0xee, 0x4a, 0x77, 0x8d, 0x16, 0xef,0x67 };
uint32_t devAddr =  ( uint32_t )0x007e6ae1;

/* LoraWan channelsmask */ 
uint16_t userChannelsMask[6]={ 0x00FF,0x0000,0x0000,0x0000,0x0000,0x0000 };

/* LoraWan region */
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;
DeviceClass_t  loraWanClass = CLASS_A;

/* Pausa tra un pacchetto e l'altro (15 secondi per i test) */
uint32_t appTxDutyCycle = 60000;

/* Impostazioni Rete */
bool overTheAirActivation = true;
bool loraWanAdr = true;         // FONDAMENTALE: Attiva l'Adaptive Data Rate
bool isTxConfirmed = true;      // Usiamo messaggi confermati per forzare la risposta del Gateway
uint8_t appPort = 2;
uint8_t confirmedNbTrials = 4;

// ======================================================================
// 📊 VARIABILE GLOBALE PER CONTARE I PACCHETTI (SOPRAVVIVE AI RIAVVII!)
// ======================================================================
RTC_DATA_ATTR int contatorePacchetti = 0;


// ======================================================================
// 📦 COSTRUZIONE DEL PAYLOAD (Dinamico)
// ======================================================================
static void prepareTxFrame( uint8_t port )
{
  contatorePacchetti++;
  
  Serial.println("\n-----------------------------------------");
  Serial.printf("Preparazione Pacchetto N. %d\n", contatorePacchetti);

  // Logica di switch della grandezza del payload
  if (contatorePacchetti <= 2) {
      appDataSize = 222;
      Serial.println(">>> FASE DI RODAGGIO: Invio 10 Byte per forzare l'ADR (Abbassamento SF)");
  } else {
      appDataSize = 243;
      Serial.println(">>> FASE A REGIME: ADR attivato. Invio payload pesante da 150 Byte!");
  }

  // Riempiamo l'array appData con dei numeri crescenti fittizi (0, 1, 2, 3...)
  for (int i = 0; i < appDataSize; i++) {
      appData[i] = i; 
  }
}

// ======================================================================
// 🚀 SETUP & LOOP
// ======================================================================
void setup() {
  Serial.begin(115200);
  
  // Pausa di sicurezza per far agganciare la porta USB Nativa del Mac all'avvio
  delay(3000);
  
  Serial.println("\n==========================================");
  Serial.println(" AVVIO TEST PAYLOAD DINAMICO (10B -> 150B)");
  Serial.println("==========================================");

  Mcu.begin();
  deviceState = DEVICE_STATE_INIT;
}

void loop()
{
  switch( deviceState )
  {
    case DEVICE_STATE_INIT:
    {
#if(LORAWAN_DEVEUI_AUTO)
      LoRaWAN.generateDeveuiByChipID();
#endif
      LoRaWAN.init(loraWanClass,loraWanRegion);
      break;
    }
    case DEVICE_STATE_JOIN:
    {
      Serial.println("Tentativo di Join OTAA...");
      LoRaWAN.join();
      break;
    }
    case DEVICE_STATE_SEND:
    {
      prepareTxFrame( appPort );
      LoRaWAN.send();
      deviceState = DEVICE_STATE_CYCLE;
      break;
    }
    case DEVICE_STATE_CYCLE:
    {
      txDutyCycleTime = appTxDutyCycle + randr( -APP_TX_DUTYCYCLE_RND, APP_TX_DUTYCYCLE_RND );
      LoRaWAN.cycle(txDutyCycleTime);
      deviceState = DEVICE_STATE_SLEEP;
      break;
    }
    case DEVICE_STATE_SLEEP:
    {
      // DISATTIVATO PER IL MAC: non usiamo il VERO sleep sennò la porta USB si stacca!
      LoRaWAN.sleep(loraWanClass);
      //delay(10);
      break;
    }
    default:
    {
      deviceState = DEVICE_STATE_INIT;
      break;
    }
  }
}