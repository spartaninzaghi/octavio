/**
 * @file utility.h
 * @author Mate Narh
 * 
 * Header file consolidating utility macros, functions, etc.
 */

#ifndef UTILITY_H
#define UTILITY_H

// ------------------------ SPI Communication Macros -----------------------

#define FSPI_MOSI MOSI  // Default: 11  
#define FSPI_MISO MISO  // Default: 13   
#define FSPI_SCLK SCK   // Default: 12 
#define FSPI_SS   SS    // Default: 10 | CS0

#define HSPI_MISO 37    // GP-SPI3Q | Q stands for Quad Data I/O (QSPI)   
#define HSPI_MOSI 35    // GP-SPI3D | D stands for Data Out
#define HSPI_SCLK 36    // GP-SPI3  | Clock 
#define HSPI_SS   45    // GP-SPI3  | Chip Select (CS)

// ---------------------------- MIDI Constants -----------------------------
#define NOTE_ON   0x90
#define NOTE_OFF  0x80

// --------------- Key ADC Pins on ESP32-S3 Devkit-C1 N16R8 ----------------
#define KEY_1     1
#define KEY_2     2
#define KEY_3     3
#define KEY_4     4
#define KEY_5     5
#define KEY_6     6
#define KEY_7     7
#define KEY_8     8
#define KEY_9     9
#define KEY_10    14
#define KEY_11    15
#define KEY_12    16

#endif // UTILITY_H