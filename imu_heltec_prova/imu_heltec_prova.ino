#include "LoRaWan_APP.h"
#include "Config.h"
#include "IMU_Handler.h"

uint8_t devEui[] = { 0x22, 0x32, 0x33, 0x00, 0x00, 0x88, 0x88, 0x02 };
uint8_t appEui[] = { 0x50, 0x3c, 0x3f, 0xaf, 0x13, 0x8a, 0xc5, 0xac };
uint8_t appKey[] = { 0x9C, 0x82, 0x92, 0xF2, 0x4E, 0x7C, 0x78, 0xFB, 0xB6, 0x1A, 0xB2, 0xE0, 0x38, 0x4E, 0x43, 0x89 };

uint8_t nwkSKey[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
uint8_t appSKey[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
uint32_t devAddr =  ( uint32_t )0;

uint16_t userChannelsMask[6] = { 0x00FF,0x0000,0x0000,0x0000,0x0000,0x0000 };
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;
DeviceClass_t  loraWanClass = CLASS_A;

// Usiamo il valore definito nel file Config.h
uint32_t appTxDutyCycle = APP_TX_DUTYCYCLE;
bool overTheAirActivation = true;
bool loraWanAdr = true;
bool isTxConfirmed = false; 
uint8_t appPort = 2;
uint8_t confirmedNbTrials = 4;

RTC_DATA_ATTR int contatorePacchetti = 0;


void setup() {
  Serial.begin(115200);
  delay(3000); 

  Serial.println("\n==========================================");
  Serial.println(" AVVIO FIRMWARE - PENDOLO MODULARE");
  Serial.println("==========================================");

  Mcu.begin(); 
  
  // Inizializzazione delegata al modulo esterno
  if(initMPU()) {
    Serial.println("[SETUP] MPU6050 inizializzato correttamente!");
  } else {
    Serial.println("[ERRORE] Impossibile trovare MPU6050. Controlla i cavi!");
    while(1) { delay(10); } // Blocco di sicurezza
  }

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
      Serial.println("[LORA] Tentativo Join OTAA...");
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
      LoRaWAN.sleep(loraWanClass);  // delay(10)
      break;
    }
    default:
    {
      deviceState = DEVICE_STATE_INIT;
      break;
    }
  }
}


static void prepareTxFrame( uint8_t port )
{
  contatorePacchetti++;
  Serial.printf("\n[LORA] Preparazione Pacchetto N. %d\n", contatorePacchetti);

  // Vogliamo 100 campionamenti a 20Hz (Finestra di 5 Secondi)
  const int NUM_CAMPIONAMENTI = 100;
  const int DELAY_MS = 50; // 50ms = 20Hz

  // Ogni intero a 16 bit occupa 2 byte. Inseriamo solo la X nel buffer.
  appDataSize = NUM_CAMPIONAMENTI * 2; 
  int indicePayload = 0;
  
  Serial.println("[MPU] Inizio campionamento 5s...");

  for(int i = 0; i < NUM_CAMPIONAMENTI; i++) {
    // Chiamata pulita al nostro file esterno
    MPUData data = readMPUData();

    // Splittiamo l'intero in due byte (Bit-Shifting) è a 16bit ma appData[] di LoRa vuole 8 bit 
    appData[indicePayload]     = (data.accX >> 8) & 0xFF; // Parte Alta
    appData[indicePayload + 1] = data.accX & 0xFF;        // Parte Bassa
    indicePayload += 2;

    delay(DELAY_MS);
  }

  Serial.println("[MPU] Buffer pronto. Passaggio alla radio LoRa...");
}