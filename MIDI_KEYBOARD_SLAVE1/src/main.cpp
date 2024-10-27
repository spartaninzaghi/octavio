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

#define HEADER    0x80
#define TIMESTAMP 0x80
#define NOTE_ON   0x90
#define NOTE_OFF  0x80

struct Key 
{
  const int pin; 
  uint8_t note;
};

struct MidiMessage
{
  uint8_t header;
  uint8_t timeStamp;
  uint8_t status;
  uint8_t note;
  uint8_t channel;
};

int value;
uint8_t status;
uint8_t velocity;

int prevKeyVelocities[12] = {0};

const Key keys[] = {
  {KEY_1,  60}, // Starts at Middle C: C3 | 0x3C
  {KEY_2,  61},
  {KEY_3,  62},
  {KEY_4,  63},
  {KEY_5,  64},
  {KEY_6,  65},
  {KEY_7,  66},
  {KEY_8,  67},
  {KEY_9,  68},
  {KEY_10, 69},
  {KEY_11, 70},
  {KEY_12, 71}
};

void setup() {

  // Sample analog pins at 12-bit resolution: 0-4095
  analogReadResolution(12);
  
  // Set up analog pins as pullup
  for (int i = 0; i < 12; i++)
  {
    pinMode(keys[i].pin, INPUT_PULLUP);
  }
}

void loop() {

  /**
   * For each key
   *   1. Extract its velocity
   *   2. if the velocity changed since the last recorded value
   *       2a. key status is NOTE_ON if velocity > 0 else NOTE_OFF
   *       2b. Wrap data into MIDI message and send to master over SPI
   */
  for (int i = 0; i < 12; i++)
  {
    value = analogRead(keys[i].pin);
    velocity = map(value, 0, 4095, 0, 127);

    bool velocityChanged = (prevKeyVelocities[i] != velocity);

    // if the state of this key changed, update its prev state to curr state
    if (velocityChanged)
    {
      status = (velocity > 0) ? NOTE_ON : NOTE_OFF;

      const MidiMessage message 
      {
        HEADER, 
        TIMESTAMP, 
        status, 
        keys[i].note,
        velocity
      };

      prevKeyVelocities[i] = velocity;
    }
  }
  
}
