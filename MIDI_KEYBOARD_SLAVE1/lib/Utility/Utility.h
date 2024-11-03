/**
 * @file utility.h
 * @author Mate Narh
 * 
 * Header file consolidating utility macros, functions, etc.
 */

#ifndef UTILITY_H
#define UTILITY_H

#include <Arduino.h>
#include <ESP32SPISlave.h>

// #define HEADER    0x80
// #define TIMESTAMP 0x80

// #define NOTE_ON   0x90
// #define NOTE_OFF  0x80

// #define VELOCITY_ON   0x7F
// #define VELOCITY_OFF  0x00

#define VSPI_MISO 19
#define VSPI_MOSI 23
#define VSPI_SCLK 18
#define VSPI_SS   5

#define HSPI_MISO 12
#define HSPI_MOSI 13
#define HSPI_SCLK 14
#define HSPI_SS   15

// BLE Wrappers
#define HEADER        0x80
#define TIMESTAMP     0x80

// MIDI Constants
#define NOTE_ON       0x90
#define NOTE_OFF      0x80
#define VELOCITY_ON   0x7F // for testing purposes
#define VELOCITY_OFF  0x00 // for testing purposes

#define HSPI_MISO 12
#define HSPI_MOSI 13
#define HSPI_SCLK 14
#define HSPI_SS   15

#define LED 2

struct MidiMessage
{
  uint8_t header;
  uint8_t timestamp;
  uint8_t status;
  uint8_t note;
  uint8_t velocity;
};

// const Key keys[] = {
//   //            Key Index
//   {KEY_1,  60}, // 0 Starts at Middle C: C3 | 0x3C
//   {KEY_2,  61}, // 1 
//   {KEY_3,  62}, // 2 
//   {KEY_4,  63}, // 3
//   {KEY_5,  64}, // 4
//   {KEY_6,  65}, // 5
//   {KEY_7,  66}, // 6
//   {KEY_8,  67}, // 7
//   {KEY_9,  68}, // 8
//   {KEY_10, 69}, // 9
//   {KEY_11, 70}, // 10
//   {KEY_12, 71}  // 11
// };



#endif // UTILITY_H