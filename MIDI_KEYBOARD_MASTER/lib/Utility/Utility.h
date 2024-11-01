/**
 * @file utility.h
 * @author Mate Narh
 * 
 * Header file consolidating utility macros, functions, etc.
 */

#ifndef UTILITY_H
#define UTILITY_H

#include <Arduino.h>

#define VSPI_MISO MISO
#define VSPI_MOSI MOSI
#define VSPI_SCLK SCK
#define VSPI_SS   SS

#define HSPI_MISO 12
#define HSPI_MOSI 13
#define HSPI_SCLK 14
#define HSPI_SS   15

#define HEADER    0x80
#define TIMESTAMP 0x80

#define NOTE_ON   0x90
#define NOTE_OFF  0x80

struct MidiMessage
{
  uint8_t header;
  uint8_t timeStamp;
  uint8_t status;
  uint8_t note;
  uint8_t velocity;
};



#endif // UTILITY_H