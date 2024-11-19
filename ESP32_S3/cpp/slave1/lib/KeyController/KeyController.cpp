/**
 * @file KeyController.cpp
 * @author Mate Narh
 */
 
#include "KeyController.h"
#include <ESP32SPIslave.h>

ESP32SPISlave* slave;

/**
 * @brief Constructor
 * @param keyCount Number of keys to control
 * @param keyPins Array of GPIO pins for the keys of this controller
 * @param baselines The zero-equivalent ADC values for the keys of this controller
 * @param startNote The start note of this controllers key range. eg. Middle C
 * @param resolution The resolution for analog reading the ACP pins of the keys for this controller
 */
KeyController::KeyController(const size_t keyCount, const int *keyPins, const int *baselines, uint8_t startNote, const int resolution)
{
    //
    // Initialize array of pointers to Keys
    //
    mKeyCount = keyCount;
    mKeys = new Key *[keyCount];

    //
    // Determine the maximum ADC value at which keys will be analogue read 
    // Bit math for: MAX ADC VALUE = 2^resolution - 1 
    //
    int maxAdcValue = (1 << resolution) - 1; 

    for (int i = 0; i < keyCount; i++)
    {
        // Set up the note for this key by adding the offset to the reference note
        uint8_t note = static_cast<uint8_t>(startNote + i);

        //
        // Create a pointer to this key and store it in the array for all keys
        //
        mKeys[i] = new Key(keyPins[i], note, maxAdcValue, baselines[i]);
    }
    Serial.println("Done setting up key controller ...");
}

/**
 * @brief Destructor
 */
KeyController::~KeyController()
{
    delete[] mKeys; // Free the memory allocated to key pointers    
}

/**
 * @brief Initialize SPI communication for this constroller
 * @param spiBus The SPI bus to communicate over: VSPI/HSPI
 * @param spiMode The spi mode for communicating with master
 * @param bufferSize The size of the buffer used in communicating with master
 * @param queueSize The size of the queue used in communicating with master
 */
void KeyController::initializeSpi(const uint8_t spiBus, const uint8_t spiMode, const size_t bufferSize, const size_t queueSize)
{
    delay(2000); // give SPI ~2 seconds to stabilize

    slave = new ESP32SPISlave;
    slave->setDataMode(SPI_MODE0);
    slave->setQueueSize(queueSize);

    mBufferSize = bufferSize;
    mQueueSize = queueSize;

    //
    // Initialize the transfer buffer for this controller
    //
    mTransferBuffer = new uint8_t [bufferSize];
    memset(mTransferBuffer, 0, bufferSize);

    //
    // Begin SPI based on given SPI bus 
    //
    if (spiBus == HSPI)
    {
        slave->begin(HSPI, HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_SS);
    }
    else if (spiBus == FSPI) 
    {
        slave->begin(FSPI, FSPI_SCLK, FSPI_MISO, FSPI_MOSI, FSPI_SS);
    }
}

/**
 * @brief Run this key controller to automatically update key data and communicate with master
 */
void KeyController::run()
{
    for (int i = 0; i < mKeyCount; i++)
    {
        Key* key = mKeys[i];

        key->Update();

        //
        // The transfer buffer, mTransferBuffer is filled in this partition:
        // -- payload 1 -> readiness (indicates whether this key has an update)
        // -- payload 2 --> velocities
        // -- payload 3 -> statuses
        //
        mTransferBuffer[i + 0 * mKeyCount] = key->IsReadyForMIDI();
        mTransferBuffer[i + 1 * mKeyCount] = key->GetVelocity();
        mTransferBuffer[i + 2 * mKeyCount] = key->GetStatus();

        //
        // Debug lines
        //
        // if (key->IsReadyForMIDI())
        // {
        //     Serial.print(" | Key: ");       Serial.print(key->GetNote());
        //     Serial.print(" | Readiness: "); Serial.print(key->IsReadyForMIDI());
        //     Serial.print(" | Velocity: ");  Serial.print(key->GetVelocity());
        //     Serial.print(" | Status: ");    Serial.print(key->GetStatus());
        //     Serial.println();
        // }
    }

    //
    // Send the packet containing the data on the keys of this controller to the master
    //
    // Using wait() instead of trigger() incorporates blocking, ensuring that the entire
    // update message sent by this KeyController is received by the master before other
    // program execution resumes.
    //
    slave->queue(mTransferBuffer, NULL, mBufferSize);
    slave->wait();
}
