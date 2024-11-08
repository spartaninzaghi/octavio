#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <SPI.h>

#include <Utility.h>

// Indicators
#define LED 2

#define SLAVE1_KEY_COUNT 2

static const int spiClk = 5000000; // 5 MHz -- max: 10 MHz but choose value < 7.5 MHz

SPIClass *hspi = NULL;

static constexpr size_t SLAVE1_BUFFER_SIZE = 8;
static const uint8_t slave1Notes[SLAVE1_KEY_COUNT] {0x3C, 0x3D}; // for testing

const Slave slave1 = {1, 2, 4};

uint8_t slave1RxBuffer[SLAVE1_BUFFER_SIZE] {0};

// Create BLE Server and Characteristic
BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;

bool deviceConnected = false;
bool previousDeviceConnected = false;

// ------------------------- Function Declarations ---------------------------
void querySlave(SPIClass *spi, const int ss, uint8_t *receiveBuffer, const size_t bufferSize);
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
  pinMode(HSPI_MOSI, OUTPUT);
  digitalWrite(HSPI_SS, HIGH);
  hspi->begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_SS);

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
    querySlave(hspi, HSPI_SS, slave1RxBuffer, SLAVE1_BUFFER_SIZE);
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
 * This function initiates SPI communication with a slave device to buffer 
 * in its current analog readings of its note velocities and statuses. It
 * only reads in the incoming slave message over MISO, without responding
 *
 * @param spi The SPIClass object to query slave on. VSPI/HSPI
 * @param ss The digital slave select pin to use for query
 * @param receiveBuffer The buffer to receive the response of the query into
 * @param bufferSize The size of the buffer to receive the query response into
 */
void querySlave(SPIClass *spi, const int ss, uint8_t *receiveBuffer, const size_t bufferSize)
{
  //
  // Master receives data from slave with the following partition
  // -----------------------------------------------------------
  // |   1 READINESS   |    2 VELOCITIES    |    3 STATUSES    |
  // -----------------------------------------------------------
  //
  spi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  digitalWrite(ss, LOW);
  spi->transferBytes(NULL, receiveBuffer, bufferSize);
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
  // ESP is little endian. Read buffer from LSB
  for (int i = 0; i < SLAVE1_KEY_COUNT; i++)
  {
    uint8_t readiness = slave1RxBuffer[i + 0 * SLAVE1_KEY_COUNT];

    if (readiness == 0x01)
    {
      uint8_t note = slave1Notes[i];

      uint8_t velocity  = slave1RxBuffer[i + 1 * SLAVE1_KEY_COUNT];
      uint8_t status    = slave1RxBuffer[i + 2 * SLAVE1_KEY_COUNT];

      uint8_t message[5] = {HEADER, TIMESTAMP, status, note, velocity};

      // Debug Lines for monitoring master receipts of SPI messages
      Serial.print("Current MIDI for note: ");
      Serial.print(note, HEX);
      Serial.print(" update: ");
      Serial.print(readiness);
      Serial.print(" | Status: ");
      Serial.print(status, HEX);
      Serial.print(" | Velocity: ");
      Serial.print(velocity);
      Serial.print(" | Current Time (s): ");
      Serial.print(millis() / 1000);
      Serial.println();

      delay(10); // wait 50ms to buffer BLE transmission

      pCharacteristic->setValue(message, sizeof(message));
      pCharacteristic->notify();
    }
  }
}