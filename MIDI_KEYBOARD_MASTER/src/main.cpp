#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <SPI.h>

// Indicators
#define LED 2

// SPI Communication: HSPI Pins
#define HSPI_MISO 12
#define HSPI_MOSI 13
#define HSPI_SCLK 14
#define HSPI_SS   15

// MIDI Constants
#define HEADER       0x80
#define TIMESTAMP    0x80
#define NOTE_ON      0x90
#define NOTE_OFF     0x80
#define VELOCITY_ON  0x7F // for testing purposes
#define VELOCITY_OFF 0x00 // for testing purposes

#define SLAVE1_KEY_COUNT 2

static const int spiClk = 1000000; // 1 MHz

SPIClass *hspi = NULL;

static constexpr size_t SLAVE1_BUFFER_SIZE = 4;
static const uint8_t slave1Notes[SLAVE1_KEY_COUNT] = {0x3C, 0x3D}; // for testing

uint8_t receivedVelocities[SLAVE1_BUFFER_SIZE];
uint8_t previousVelocities[SLAVE1_BUFFER_SIZE];

struct MidiMessage
{
  uint8_t header;
  uint8_t timeStamp;
  uint8_t status;
  uint8_t note;
  uint8_t velocity;
};

// BLE UUIDs
#define SERVICE_UUID "03B80E5A-EDE8-4B33-A751-6CE34EC4C700"
#define CHARACTERISTIC_UUID "7772E5DB-3868-4112-A1A9-F2669D106BF3"

// Create BLE Server and Characteristic
BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;

bool deviceConnected = false;
bool previousDeviceConnected = false;

// ------------------------- Function Declarations ---------------------------
void querySlave(SPIClass *spi, const int ss);
void sendMidiMsgUpdatesOverBLE();

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    deviceConnected = true;
  }

  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false;
    pServer->startAdvertising();
  }
};

void setup()
{
  Serial.begin(115200);

  // Initialize indicators
  pinMode(LED, OUTPUT);

  //
  // ------------------------------ SPI Setup -----------------------------
  //

  // Initialize SPI and wait 2 seconds (2000 ms) to guarantee robust setup
  hspi = new SPIClass(HSPI);

  delay(2000);

  pinMode(HSPI_SS, OUTPUT);
  digitalWrite(HSPI_SS, HIGH);
  hspi->begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_SS);

  memset(previousVelocities, 0, SLAVE1_BUFFER_SIZE);
  memset(receivedVelocities, 0, SLAVE1_BUFFER_SIZE);

  //
  // ----------------------------- BLE Setup -----------------------------
  //

  // Initialize BLE Device
  BLEDevice::init("Octavio MIDI");

  // Create BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create BLE Service
  BLEService *pService = pServer->createService(BLEUUID(SERVICE_UUID));

  // Create BLE Characteristic with read, write-no-response, and notify properties
  pCharacteristic = pService->createCharacteristic(
    BLEUUID(CHARACTERISTIC_UUID),
    BLECharacteristic::PROPERTY_READ     |
    BLECharacteristic::PROPERTY_WRITE_NR |
    BLECharacteristic::PROPERTY_NOTIFY   
  );

  // Add the descriptor for notifications
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the BLE service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(pService->getUUID());
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); // Minimum connection time: 
  pAdvertising->setMaxPreferred(0x12); //
  pAdvertising->start();
  Serial.println("Waiting for a client connection...");
}

void loop()
{
  //
  //--- Handle MIDI message transmission based on connection status ---
  //

  // Connected: Notify client of changed value
  if (deviceConnected)
  {
    querySlave(hspi, HSPI_SS);
    delay(10); // delay 10 milliseconds as slave responds
    sendMidiMsgUpdatesOverBLE();
    previousDeviceConnected = true; // Update the previous connection state
  }

  // Disconnecting
  if (!deviceConnected && previousDeviceConnected)
  {
    delay(500);
    pServer->startAdvertising();
    Serial.println("start advertising");
    previousDeviceConnected = deviceConnected;
  }

  // connecting
  if (deviceConnected && !previousDeviceConnected)
  {
    // do stuff here on connecting
    previousDeviceConnected = deviceConnected;
  } 
}

/**
 * @brief Query a slave ESP32 for it note velocity readings
 *
 * This function initiates SPI communication with a slave device
 * to buffer in its current analog readings of its note velocities.
 * It only reads in the incoming slave message over MISO, without
 * responding
 *
 * @param spi The SPIClass object to query slave on. VSPI/HSPI
 * @param ss The digital slave select pin to use for query
 */
void querySlave(SPIClass *spi, const int ss)
{
  spi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  digitalWrite(ss, LOW);
  spi->transferBytes(NULL, receivedVelocities, SLAVE1_BUFFER_SIZE);
  digitalWrite(ss, HIGH);
  spi->endTransaction();
}

/**
 * @brief Send ~real-time MIDI messages over BLE to connected device
 *
 * This function pipes MIDI messages to connected device over BLE, if
 * the note velocity of any key changed. The transition condition helps
 * reduce traffic.
 */
void sendMidiMsgUpdatesOverBLE()
{
  // for (int i = 0; i < SLAVE1_KEY_COUNT; i++)
  // {
  //   Serial.println("Key: ");
  //   Serial.println(slave1Notes[i]);
  //   Serial.println("Received: ");
  //   Serial.println((int)receivedVelocities[i]);
  //   Serial.println("Previous: ");
  //   Serial.println((int)previousVelocities[i]);

  // }
  if (memcmp(receivedVelocities, previousVelocities, SLAVE1_BUFFER_SIZE) != 0)
  {
    for (int i = 2; i < SLAVE1_KEY_COUNT; i++)
    {
      bool noteVelocityChanged = (receivedVelocities[i] != previousVelocities[i]);

      if (noteVelocityChanged)
      {
        uint8_t status = (receivedVelocities[i] > 0) ? NOTE_ON : NOTE_OFF;
        uint8_t message[5] = {HEADER, TIMESTAMP, status, slave1Notes[i], receivedVelocities[i]};
        delay(50);
        pCharacteristic->setValue(message, sizeof(message));
        pCharacteristic->notify();

        previousVelocities[i] = receivedVelocities[i];

        Serial.print("Key: ");
        Serial.print(slave1Notes[i]);
        Serial.print(" | Received: ");
        Serial.print((int)receivedVelocities[i]);
        Serial.print(" | Previous: ");
        Serial.print((int)previousVelocities[i]);
        Serial.println();
      }
    }
  }
}
