/**
 * @file utility.h
 * @author Mate Narh
 * 
 * Header file consolidating utility macros, functions, etc.
 */

#ifndef UTILITY_H
#define UTILITY_H

// SPI Communication Macros

#define FSPI_MOSI MOSI  // Default: 11  
#define FSPI_MISO MISO  // Default: 13   
#define FSPI_SCLK SCK   // Default: 12 
#define FSPI_SS   SS    // Default: 10 | CS0

#define HSPI_MISO 37    // GP-SPI3Q | Q stands for Quad Data I/O (QSPI)   
#define HSPI_MOSI 35    // GP-SPI3D | D stands for Data Out
#define HSPI_SCLK 36    // GP-SPI3  | Clock 
#define HSPI_SS   45    // GP-SPI3  | Chip Select (CS)

// MIDI Constants
#define VELOCITY_ON  0x7F // for testing purposes
#define VELOCITY_OFF 0x00 // for testing purposes
#define MIN_VELOCITY 0x0A // Velocity: 10 to filter out noisy presses

#define HEADER    0x80
#define TIMESTAMP 0x80

#define NOTE_ON   0x90
#define NOTE_OFF  0x80

#endif // UTILITY_H