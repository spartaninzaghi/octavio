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
#include <ESP32SPISlave.h>

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

#define HSPI_MISO 12
#define HSPI_MOSI 13
#define HSPI_SCLK 14
#define HSPI_SS   15

// Key constant helpers
#define KEY_COUNT   2 // for testing purposes
#define START_INDEX 0
#define STOP_INDEX  0

struct Key
{
  const int pin;
  uint8_t note;
};

ESP32SPISlave slave;

static constexpr size_t BUFFER_SIZE = 4;
static constexpr size_t QUEUE_SIZE = 1;

uint8_t noteVelocities[BUFFER_SIZE];

const int keys[KEY_COUNT] {KEY_3, KEY_4};

// Function declarations
void pollNoteVelocities();

void setup()
{
  Serial.begin(115200);

  // Sample analog pins at 12-bit resolution: 0-4095
  analogReadResolution(12);

  pinMode(LED, OUTPUT);

  for (int i = 0; i < BUFFER_SIZE; i++)
  {
    noteVelocities[i] = 0x00;
  }
  delay(2000);

  // initiate spi instance
  pinMode(HSPI_MISO, OUTPUT);
  slave.setDataMode(SPI_MODE0);
  slave.setQueueSize(QUEUE_SIZE);

  slave.begin(HSPI, HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_SS);
}

void loop()
{
  pollNoteVelocities();
  slave.queue(noteVelocities, NULL, BUFFER_SIZE);
  slave.wait();
}

/**
 * @brief Update the velocity of each note of this slave in readiness for master query
 * 
 * This function reads the current analog value of each note's velocity and feeds into
 * the `noteVelocities` buffer.
 * @return
 */
void pollNoteVelocities()
{
  for (int i = 0; i < KEY_COUNT; i++)
  {
    int analogValue = analogRead(keys[i]);
    noteVelocities[i] = static_cast<uint8_t>(map(analogValue, 0, 4095, 0, 127));

    Serial.print("Key "); // Print which key is being read
    Serial.print(i);
    Serial.print(": ");
    Serial.print((int)noteVelocities[i]); // Print the velocity
    Serial.print(" | ");
  }
  Serial.println();
}