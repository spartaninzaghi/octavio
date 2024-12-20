#include <Arduino.h>
#include <USB.h>
#include <USBMIDI.h>
#include <SPI.h>

#include <Slave.h>
#include <RotaryEncoder.h>
#include <Wheel.h>
#include <Utility.h>

#define CHANNEL      1 // Range: 1 - 16 channels available
#define CABLE_NUMBER 1

#define ADC_RESOLUTION 10

#define KEY_COUNT1 2
#define KEY_COUNT2 2

#define TRANSPOSE_MAX  24
#define TRANSPOSE_MIN -24

#define TRANSPOSE_CLK  15
#define TRANSPOSE_DT   16
#define TRANSPOSE_SW   17

#define PITCHBEND_PIN  4
#define PITCHBEND_MIN -8192
#define PITCHBEND_MAX  8191

#define MODULATION_PIN  5
#define MODULATION_MIN  0
#define MODULATION_MAX  127
#define MODULATION_CC   1

// ------------------------ Peripheral initialization -------------------------
Wheel *pitchWheel = nullptr;
Wheel *modulationWheel = nullptr;
RotaryEncoder *transposeKnob = nullptr;

// ----------------------------- Set up SPI macros ----------------------------

static const uint32_t spiClock = 10000000; // 10 MHz -- max: 10 MHz but choose value < 7.5 MHz

SPIClass *fspi = nullptr;
SPIClass *hspi = nullptr;

// ----------------------------- Slave preparation ---------------------------
Slave* slave1 = nullptr;
Slave* slave2 = nullptr;

static constexpr size_t BUFFER_SIZE1 = 8;
static constexpr size_t BUFFER_SIZE2 = 8;

const uint8_t notes1[KEY_COUNT1] {0x3C, 0x3D}; // {C4, C4#} for testing
const uint8_t notes2[KEY_COUNT2] {0x3C, 0x3D}; // {C4, C4#} for testing

uint8_t rxBuffer1[BUFFER_SIZE1] {0};
uint8_t rxBuffer2[BUFFER_SIZE2] {0};

// Create USBMIDI instance
USBMIDI usbMIDI;

// --------------------------- Function Declarations -----------------------------
void querySlaves();
void sendMidiMsgUpdatesOverUSB();

void setup()
{
  Serial.begin(115200);

  // Read analog data at 12-bit resolution (Range: 0 - 4095)
  analogReadResolution(ADC_RESOLUTION);

  //
  // ----------------------- USB & USBMIDI Initialization -----------------------
  //
  USB.begin();
  usbMIDI.begin();
 
  //
  // Initialize indicators
  //
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  //
  // ---------------------------- Pitch Bend Wheel Setup -------------------------------
  //
  // No ADC pin setup required for pitch bend wheel. Simply use values directly
  //
  pitchWheel = new Wheel(PITCHBEND_PIN, ADC_RESOLUTION, PITCHBEND_MIN, PITCHBEND_MAX);

  //
  // ---------------------------- Modulation Wheel Setup -------------------------------
  //
  // No ADC pin setup required for modulation wheel. Simply use values directly
  //
  modulationWheel = new Wheel(MODULATION_PIN, ADC_RESOLUTION, MODULATION_MIN, MODULATION_MAX);

  //
  // ---------------------------- Transpose Setup -------------------------------
  //
  pinMode(TRANSPOSE_CLK, INPUT);
  pinMode(TRANSPOSE_DT, INPUT);
  pinMode(TRANSPOSE_SW, INPUT_PULLUP);

  transposeKnob = new RotaryEncoder(TRANSPOSE_CLK, TRANSPOSE_DT, TRANSPOSE_SW, TRANSPOSE_MAX, TRANSPOSE_MIN);

  //
  // ------------------------------ SPI Setup ------------------------------
  //
  // Initialize SPI and wait 2 seconds (2000 ms) to stabilize setup
  //
  fspi = new SPIClass(FSPI);
  hspi = new SPIClass(HSPI);

  delay(2000);

  //
  // GP-SPI2: FSPI -> Slave 1
  //
  pinMode(FSPI_SS, OUTPUT);
  pinMode(FSPI_MOSI, OUTPUT);
  digitalWrite(FSPI_SS, HIGH);
  fspi->begin(FSPI_SCLK, FSPI_MISO, FSPI_MOSI, FSPI_SS);
  Serial.println("SPI setup complete for slave 1 | Type: GP-SPI2 / FSPI");
  
  //
  // GP-SPI3: HSPI -> Slave 2
  //
  pinMode(HSPI_SS, OUTPUT);
  pinMode(HSPI_MOSI, OUTPUT);
  digitalWrite(HSPI_SS, HIGH);
  hspi->begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_SS);
  Serial.println("SPI setup complete for slave 2 | Type: GP-SPI3 / HSPI");

  //
  // ----------------------- Buffer Initizialization ----------------------
  //
  memset(rxBuffer1, 0, BUFFER_SIZE1);
  memset(rxBuffer2, 0, BUFFER_SIZE2);

  //
  // ---------------------------- Slave Setup -----------------------------
  //
  slave1 = new Slave(1, KEY_COUNT1, notes1);
  slave2 = new Slave(2, KEY_COUNT2, notes2);

  slave1->SetSpiParameters(fspi, spiClock, MSBFIRST, SPI_MODE0, BUFFER_SIZE1, rxBuffer1);
  slave2->SetSpiParameters(hspi, spiClock, MSBFIRST, SPI_MODE0, BUFFER_SIZE2, rxBuffer2);
}

void loop()
{
  //
  // ------------------------- Update Peripherals -------------------------
  //
  pitchWheel->Update();
  modulationWheel->Update();
  transposeKnob->Update();

  //
  //--- Handle MIDI message transmission based on USB connection status ---
  //

  //
  // Connected: Send MIDI to host over USB
  //
  querySlaves();
  sendMidiMsgUpdatesOverUSB();
}

/**
 * @brief Query a slave ESP32 for it note velocity readings
 *
 * This function initiates SPI communication with a slave device to buffer 
 * in its current analog readings of its note velocities and statuses. It
 * only reads in the incoming slave message over MISO, without responding
 */
void querySlaves()
{
  //
  // Master receives data from its slaves with the following partition
  // -----------------------------------------------------------
  // |   1 READINESS   |    2 VELOCITIES    |    3 STATUSES    |
  // -----------------------------------------------------------
  //

  //
  // After each slave completes its query, the receive buffer corresponding to that
  // slave is updated such that:
  //
  // rxBuffer1: updates for slave 1
  // rxBuffer2: updates for slave 2
  //
  // slave1->querySPIPeerOnOtherSide();
  slave2->querySPIPeerOnOtherSide();
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
  // Serial.println(pitchWheel->GetBend());
  //
  // ------------------------ Send Pitch Bend Message ---------------------------
  //
  if (pitchWheel->IsReadingChanged())
  {
    usbMIDI.pitchBend(pitchWheel->GetReading(), CHANNEL);
  }

  //
  // ------------------ Send Modulation (MIDI CC 1) Message -------------------
  //
  if (modulationWheel->IsReadingChanged())
  {
    usbMIDI.controlChange(MODULATION_CC, (uint8_t)modulationWheel->GetReading(), CHANNEL);
  }
  
  // ESP is little endian. Read buffer from LSB

  //
  // ------------------------ Slave 1 MIDI Transmission -------------------------
  //
  // for (int i = 0; i < KEY_COUNT1; i++)
  // {
  //   uint8_t readiness = rxBuffer1[i + 0 * KEY_COUNT1];

  //   if (readiness == 0x01)
  //   {
  //     uint8_t note = notes1[i];

  //     uint8_t velocity  = rxBuffer1[i + 1 * KEY_COUNT1];
  //     uint8_t status    = rxBuffer1[i + 2 * KEY_COUNT1];

  //     if (status == NOTE_ON)
  //     {
  //       usbMIDI.noteOn(note, velocity, CHANNEL);
  //     } 
  //     else
  //     {
  //       usbMIDI.noteOff(note, velocity, CHANNEL);
  //     }
  //   }
  // }

  //
  // ------------------------ Slave 2 MIDI Transmission -------------------------
  //
  for (int i = 0; i < KEY_COUNT2; i++)
  {
    uint8_t readiness = rxBuffer2[i + 0 * KEY_COUNT2];

    if (readiness == 0x01)
    {
      //
      // Only send a PITCH BEND after a NOTE
      // 
      // Transpose NOTE before sending it. 
      //
      // If the current transpose counter is 0, the note is sent unaltered
      // Otherwise, note + counter is sent. eg. C4 becomes C4# for a count
      // value of 1, meaning transpose by 1 semitone
      //
      uint8_t note = notes2[i] + transposeKnob->GetCounter();

      uint8_t velocity  = rxBuffer2[i + 1 * KEY_COUNT2];
      uint8_t status    = rxBuffer2[i + 2 * KEY_COUNT2];

      if (status == NOTE_ON)
      {
        usbMIDI.noteOn(note, velocity, CHANNEL);
      } 
      else
      {
        usbMIDI.noteOff(note, velocity, CHANNEL);
      }
    }
  }

  // memset(rxBuffer1, 0, BUFFER_SIZE1);
  // memset(rxBuffer2, 0, BUFFER_SIZE2);
}