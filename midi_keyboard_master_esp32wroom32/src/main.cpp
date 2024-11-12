#include <Arduino.h>
#include <BLEMIDI_Transport.h>
#include <hardware/BLEMIDI_ESP32_NimBLE.h>
#include <SPI.h>

#include <Utility.h>

// Indicators
#define LED 2

// MIDI Macros
#define CHANNEL 1 // Range: 0 - 15 (16 channels available)

// Set up SPI macros and buffers
#define SLAVE1_KEY_COUNT 2

static const int spiClk = 5000000; // 5 MHz -- max: 10 MHz but choose value < 7.5 MHz

SPIClass *hspi = NULL;

static constexpr size_t SLAVE1_BUFFER_SIZE = 8;
static const uint8_t slave1Notes[SLAVE1_KEY_COUNT] {0x3C, 0x3D}; // for testing

uint8_t slave1RxBuffer[SLAVE1_BUFFER_SIZE] {0};

// Create BLE Server and Characteristic
BLEMIDI_CREATE_INSTANCE("Octavio MIDI", MIDI);
bool deviceConnected = false;

// ------------------------- Function Declarations ---------------------------
void OnConnect();
void OnDisconnect();
void sendMidiMsgUpdatesOverBLE();
void querySlave(SPIClass *spi, const int ss, uint8_t *receiveBuffer, const size_t bufferSize);


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

  delay(4000);

  pinMode(HSPI_SS, OUTPUT);
  pinMode(HSPI_MOSI, OUTPUT);
  digitalWrite(HSPI_SS, HIGH);
  hspi->begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_SS);

  //
  // ----------------------------- BLE Setup -----------------------------
  //
  BLEMIDI.setHandleConnected(OnConnect);
  BLEMIDI.setHandleDisconnected(OnDisconnect);
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
 * @brief Callback function for when a bluetooth connection is successfully made
 */
void OnConnect()
{
  deviceConnected = true;
}

/**
 * @brief Callback function for when a bluetooth connection is terminated
 */
void OnDisconnect()
{
  deviceConnected = false;
  // restart advertising or check how that is accounted for in Library
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

      if (status == NOTE_ON)
      {
        MIDI.sendNoteOn(note, velocity, CHANNEL);
      } 
      else
      {
        MIDI.sendNoteOff(note, velocity, CHANNEL);
      }
    }
  }
}