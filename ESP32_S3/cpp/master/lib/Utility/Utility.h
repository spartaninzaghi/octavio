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

// SPI Communication: HSPI Pins
#define HSPI_MISO 12
#define HSPI_MOSI 13
#define HSPI_SCLK 14
#define HSPI_SS   15

// MIDI Constants
#define VELOCITY_ON  0x7F // for testing purposes
#define VELOCITY_OFF 0x00 // for testing purposes
#define MIN_VELOCITY 0x0A // Velocity: 10 to filter out noisy presses

#define HEADER    0x80
#define TIMESTAMP 0x80

#define NOTE_ON   0x90
#define NOTE_OFF  0x80

struct SlaveInfo 
{
    const int id;
    const int keyCount;
    uint8_t* notes;
};

#endif // UTILITY_H