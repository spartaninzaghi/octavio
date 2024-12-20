/**
 * @file main.cpp
 * @author Mate Narh
 *
 * Driver code for Slave 1 ESP32-S3 N16R8 25-Key MIDI keyboard
 *
 * Slave 1 handles I/O for the first octave of the keyboard. i.e.
 * the first 12 keys: 1 - 12
 */
#include <Arduino.h>
#include <KeyController.h>
#include <Utility.h>

// Key constant helpers
#define KEY_COUNT   2    // for testing purposes
#define START_NOTE  0x3C // Middle C: C4 | (int) 60 

#define RESOLUTION  12

#define SPI_MISO HSPI_MISO
#define SPI_BUS  HSPI
#define SPI_MODE SPI_MODE0

#define THRESHOLD   5

static constexpr size_t BUFFER_SIZE = 8; // Size of buffer to hold tx rx data | should be at least key count
static constexpr size_t QUEUE_SIZE = 1;  // Num of transaction b/n slave & master

KeyController* octave = nullptr;

const int keyPins[KEY_COUNT] {KEY_10, KEY_7};
const int damperPins[KEY_COUNT] {DAMPER_1, DAMPER_2};

void setup()
{

  // Sample analog pins at 12-bit resolution: 0-4095
  analogSetAttenuation(ADC_11db);
  analogReadResolution(RESOLUTION);

  
  Serial.begin(115200);
  Serial.println("Starting setup ... ");

  //
  // Set up damper pins as input pullup pins
  //
  // Key pins do not need setup as they are ADC pins and can be read on demand
  for (int i = 0; i < KEY_COUNT; i++)
  {
    pinMode(damperPins[i], INPUT);
  }

  pinMode(LED_BUILTIN, OUTPUT);

  //
  // Create new key controller
  //
  octave = new KeyController(KEY_COUNT, keyPins, damperPins, THRESHOLD, START_NOTE, RESOLUTION);
  
  // initiate spi instance
  pinMode(SPI_MISO, OUTPUT);
  octave->initializeSpi(SPI_BUS, SPI_MODE, BUFFER_SIZE, QUEUE_SIZE);
  Serial.println("Setup complete!...");
}

void loop()
{
  octave->run();
}