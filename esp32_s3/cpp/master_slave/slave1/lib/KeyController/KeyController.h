/**
 * @file KeyController.h
 * @author Mate Narh
 * 
 * Class for controlling keys spanning a particular range of the
 * MIDI Keyboard
 * 
 * --Tentative
 */

#ifndef KEY_CONTROLLER_H
#define KEY_CONTROLLER_H

#include <Key.h>
#include <Utility.h>

class KeyController
{
private:
    /// An array pointers to keys controlled by this controller
    Key** mKeys = nullptr;        

    ///< The number of keys this controller controls
    size_t mKeyCount = 0;  
    
    ///< The buffer size that this controllers uses for SPI communication with its master
    size_t mBufferSize = 0;  

    ///< The queue size that this controllers uses for SPI communication with its master
    size_t mQueueSize = 0;

    ///< The transfer buffer this controller uses to send polled data to master over SPI
    uint8_t* mTransferBuffer = nullptr;


public:

    KeyController(const size_t keyCount, const int* keyPins, const int* damperPins, const int threshold, uint8_t startNote, const int resoluiton);
    ~KeyController();

    void run();
    void initializeSpi(const uint8_t spiBus, const uint8_t spiMode, const size_t bufferSize, const size_t queueSize);

    KeyController() = delete;
    KeyController(const KeyController &) = delete;
    void operator=(const KeyController &) = delete;
};

#endif // KEY_CONTROLLER_H