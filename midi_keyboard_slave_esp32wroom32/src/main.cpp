/**
 * @file main.cpp
 * @author Mate Narh
 *
 * Driver code for Slave 1 ESP32-WROOM-32 25-Key MIDI keyboard
 *
 * Slave 1 handles I/O for the first octave of the keyboard. i.e.
 * the first 12 keys: 1 - 12
 *
 * With the capabilities of the ESP32-S3 MCU, more peripherals
 * can be attached to this implementation to expand features.
 */
#include <Arduino.h>
#include <KeyController.h>

// Key ADC Pins on 
#define KEY_1  36
#define KEY_2  39
#define KEY_3  34
#define KEY_4  35
#define KEY_5  32
#define KEY_6  33
#define KEY_7  25
#define KEY_8  26
#define KEY_9  27
#define KEY_10 14
#define KEY_11 12
#define KEY_12 13

#define LED 2

// Key constant helpers
#define KEY_COUNT   2    // for testing purposes
#define START_NOTE  0x3C // Middle C: C4 | (int) 60 

#define RESOLUTION  12

#define SPI_MISO HSPI_MISO
#define SPI_BUS  HSPI
#define SPI_MODE SPI_MODE0

static constexpr size_t BUFFER_SIZE = 8; // Size of buffer to hold tx rx data | should be at least key count
static constexpr size_t QUEUE_SIZE = 1;  // Num of transaction b/n slave & master

KeyController* octave = nullptr;

const int pins[KEY_COUNT] {KEY_3, KEY_4};

// --------------------------- Function declarations ---------------------------

void setup()
{
    // Sample analog pins at 12-bit resolution: 0-4095
  analogReadResolution(RESOLUTION);
  
  Serial.begin(115200);

  pinMode(LED, OUTPUT);

  //
  // Create new key controller
  //
  octave = new KeyController(KEY_COUNT, pins, START_NOTE, RESOLUTION);
  
  // initiate spi instance
  pinMode(SPI_MISO, OUTPUT);
  octave->initializeSpi(SPI_BUS, SPI_MODE, BUFFER_SIZE, QUEUE_SIZE);
}

void loop()
{
  octave->run();
}