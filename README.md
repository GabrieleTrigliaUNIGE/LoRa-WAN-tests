# LoRaWAN Infrastructure Testing

Repository contenente il codice sorgente e la documentazione relativi all'implementazione e al collaudo di un'infrastruttura di rete LoRaWAN. Il progetto è sviluppato nell'ambito di un lavoro di tesi e si concentra sull'interfacciamento tra nodi sensori (end-nodes) e un gateway industriale dotato di Network Server integrato.

## Architettura Hardware

* **Gateway:** RAK WisGate Edge Pro (operante con Network Server locale integrato).
* **End-Nodes:** Arduino MKR WAN 1310 (dotato di modulo radio Murata CMWX1ZZABZ).
* *Sviluppi futuri:* Integrazione e test di ulteriori architetture microcontrollore (es. ESP32, STM32).

## Struttura della Repository

Lo sviluppo del firmware è gestito interamente tramite l'ecosistema **PlatformIO**. Ogni cartella radice rappresenta un progetto indipendente con il proprio ambiente di compilazione:

* `Arduino_MKRWAN1310/`: Progetto PlatformIO dedicato all'Arduino MKR WAN 1310.
    * `src/`: Codice sorgente principale in C++ (es. `main.cpp`).
    * `include/`: File di intestazione (header), inclusi i parametri crittografici della rete (`arduino_secrets.h`).
    * `platformio.ini`: File di configurazione dell'ambiente, definizione della board e delle dipendenze.
* `docs/`: Documentazione tecnica, datasheet dell'hardware e dettagli di configurazione del gateway.

## Compilazione e Deployment

L'ambiente di sviluppo richiede [PlatformIO](https://platformio.org/), tipicamente utilizzato come estensione all'interno di Visual Studio Code.

La gestione delle dipendenze (es. il core Atmel SAMD e la libreria ufficiale `MKRWAN`) è automatizzata dal framework. Un esempio di configurazione base presente nel file `platformio.ini`:

```ini
[env:mkrwan1310]
platform = atmelsam
board = mkrwan1310
framework = arduino
monitor_speed = 115200
lib_deps = 
    arduino-libraries/MKRWAN@^1.1.0