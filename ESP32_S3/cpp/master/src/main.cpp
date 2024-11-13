#include <Arduino.h>
#include <USB-MIDI.h>
#include <SPI.h>

#include <Slave.h>
#include <Utility.h>

#define LED              2

#define CHANNEL          1 // Range: 1 - 16 channels available
#define CABLE_NUMBER     1

#define KEY_COUNT1 2
#define KEY_COUNT2 2

// ----------------------------- Set up SPI macros ----------------------------

static const int spiClock = 5000000; // 5 MHz -- max: 10 MHz but choose value < 7.5 MHz

SPIClass *hspi = nullptr;
SPIClass *vspi = nullptr;

// ----------------------------- Slave preparation ---------------------------
Slave* slave1 = nullptr;
Slave* slave2 = nullptr;

static constexpr size_t BUFFER_SIZE1 = 8;
static constexpr size_t BUFFER_SIZE2 = 8;

static const uint8_t notes1[KEY_COUNT1] {0x3C, 0x3D}; // for testing
static const uint8_t notes2[KEY_COUNT2] {0x3E, 0x3F}; // for testing

uint8_t rxBuffer1[BUFFER_SIZE1] {0};
uint8_t rxBuffer2[BUFFER_SIZE2] {0};

// Create USB2IDI i;nstance
USBMIDI_CREATE_INSTANCE(CABLE_NUMBER, MIDI);

// ------------------------- Function Declarations ---------------------------
void querySlave(SPIClass *spi, const int ss, uint8_t *receiveBuffer, const size_t bufferSize);
void sendMidiMsgUpdatesOverUSB();

void setup()
{
  //
  // Initiate USB MIDI instance to listen to all channels
  //
  MIDI.begin(MIDI_CHANNEL_OMNI);

  //
  // Initialize indicators
  //
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  //
  // ---------------------------- Slave Setup -----------------------------
  //
  // slave1 = new Slave(1, KEY_COUNT1, notes1, rxBuffer1);
  // slave2 = new Slave(2, KEY_COUNT2, notes2, rxBuffer2);


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
}

void loop()
{
  MIDI.read();

  //
  //--- Handle MIDI message transmission based on USB connection status ---
  //

  //
  // Connected: Send MIDI to host over USB
  //
    querySlave(hspi, HSPI_SS, rxBuffer1, BUFFER_SIZE1);
    sendMidiMsgUpdatesOverUSB();
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
  /* TODO: Change to querySlaves */
  
  //
  // Master receives data from slave with the following partition
  // -----------------------------------------------------------
  // |   1 READINESS   |    2 VELOCITIES    |    3 STATUSES    |
  // -----------------------------------------------------------
  //
  spi->beginTransaction(SPISettings(spiClock, MSBFIRST, SPI_MODE0));
  digitalWrite(ss, LOW);
  spi->transferBytes(NULL, receiveBuffer, bufferSize);
  digitalWrite(ss, HIGH);
  spi->endTransaction();
}


/**
 * @brief Send ~real-time MIDI messages over USB to connected device
 *
 * This function pipes MIDI messages to connected device over USB, if
 * the note velocity of any key changed. The transition condition helps
 * reduce traffic.
 */
void sendMidiMsgUpdatesOverUSB()
{
  /** TODO: iterate over list of slaves */
  // ESP is little endian. Read buffer from LSB
  for (int i = 0; i < KEY_COUNT1; i++)
  {
    uint8_t readiness = rxBuffer1[i + 0 * KEY_COUNT1];

    if (readiness == 0x01)
    {
      uint8_t note = notes1[i];

      uint8_t velocity  = rxBuffer1[i + 1 * KEY_COUNT1];
      uint8_t status    = rxBuffer2[i + 2 * KEY_COUNT1];

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