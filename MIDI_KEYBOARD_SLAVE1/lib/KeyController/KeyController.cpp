/**
 * @file KeyController.cpp
 * @author Mate Narh
 */
 
#include "KeyController.h"
#include <ESP32SPIslave.h>

ESP32SPISlave* slave;

static constexpr size_t BUFFER_SIZE = 4; // Size of buffer to hold tx rx data
static constexpr size_t QUEUE_SIZE = 1;  // Num of transaction b/n slave & master

uint8_t noteVelocities[BUFFER_SIZE];

/**
 * @brief Constructor
 * @param keyCount Number of keys to control
 * @param pins Array of GPIO pins for the keys of this controller
 * @param startNote The start note of this controllers key range. eg. Middle C
 * @param resolution The resolution for analog reading the ACP pins of the keys for this controller
 * @param threshold The threshold of values to discount as noise during analog reads
 */
KeyController::KeyController(const size_t keyCount, const int *pins, uint8_t startNote, const int resolution, const int threshold)
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
        Serial.print("Current pin: ");
        Serial.print(pins[i]);
        Serial.println();
        mKeys[i] = new Key(pins[i], note, maxAdcValue, threshold);
    }
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
void KeyController::initializeSpi(const uint8_t spiBus, const int spiMode, const size_t bufferSize, const size_t queueSize)
{
    delay(2000);

    slave = new ESP32SPISlave;
    slave->setDataMode(SPI_MODE0);
    slave->setQueueSize(queueSize);

    mBufferSize = bufferSize;
    mQueueSize = queueSize;

    if (spiBus == HSPI)
    {
        slave->begin(HSPI, HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_SS);
    }
    else if (spiBus == VSPI) 
    {
        slave->begin(VSPI, VSPI_SCLK, VSPI_MISO, VSPI_MOSI, VSPI_SS);
    }
    Serial.println("Done setting up SPI");
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

        uint8_t* message = key->GetCurrentMessageForMaster();

        //
        // if there is currently no transaction in flight b/n slave and master
        // and all results have been handled, queue new transactions
        //
        if (slave->hasTransactionsCompletedAndAllResultsHandled())
        {
            if (message[0] == 0x01)
            {
                Serial.print(" | Status: ");
                Serial.print(message[1]);
                Serial.print(" | Velocity: ");
                Serial.print(message[2]);
                Serial.println();
            }
            
            slave->queue(message, NULL, mBufferSize);
            slave->trigger();
        }
        //
        // if all transactions are complete and all results (from master) are
        // ready, handle results
        //
        if (slave->hasTransactionsCompletedAndAllResultsReady(mQueueSize))
        {
            const std::vector<size_t> receivedBytes = slave->numBytesReceivedAll();
        }
    }
}
