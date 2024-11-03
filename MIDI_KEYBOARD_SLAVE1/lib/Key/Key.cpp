/**
 * @file Key.cpp
 * @author Mate Narh
 * 
 * --Tentative Key Lib
 */

#include "Key.h"

/**
 * Constructor
 * @param pin The GPIO pin bound to this key
 * @param note The note that this key sounds (0-127)
 */
Key::Key(const int pin, const uint8_t note, const int maxAdcValue, const int threshold) : 
    mPin(pin), mNote(note), mMaxAdcValue(maxAdcValue), mThreshold(threshold) {}

/**
 * Bind this key to specified GPIO pin
 * @param pin The GPIO pin bound to this key
 */
void Key::SetPin(const int pin)
{
    mPin = pin;
}

/**
 * @brief Set the current status of this key
 * @param status The status of this key. NoteOn if true, else NoteOff
 */
void Key::SetStatus(uint8_t status)
{
    mStatus = status;
}

/**
 * @brief Set the velocity of this key
 * @param velocity The velocity to set to
 */
void Key::SetVelocity(uint8_t velocity)
{
    mVelocity = velocity;
}

/**
 * @brief Set whether the note of this key has been sent or not
 * @param sent 
 */
void Key::SetNoteOnSent(bool sent)
{
    mNoteOnSent = true;
}

/**
 * @brief Return the GPIO pin that this key is bound to
 */
int Key::GetPin()
{
    return mPin;
}

/**
 * @brief Return the status of this key. False implies NoteOff, else NoteOn
 */
uint8_t Key::GetStatus()
{
    return mStatus;
}

/**
 * @brief Return the current velocity of this key
 */
uint8_t Key::GetVelocity()
{
    return mVelocity;
}

/**
 * @brief Return the note that this key sounds 
 */
uint8_t Key::GetNote()
{
    return mNote;
}

/**
 * @brief Return whether this key is ready with new MIDI data
 */
bool Key::IsReadyForMidi()
{
    return mReadyForMidi;
}

/**
 * @brief Update the status and velocity of this key.
 */
void Key::Update()
{
    int value = analogRead(mPin);
    uint8_t velocity = map(value, 0, 4095, 0, 127);

    //
    // Velocity is in valid range, send NOTE ON message if it is the peak and
    // a NOTE ON message has not been sent already
    //
    if (velocity > mThreshold)
    {
        //
        // Prepare to send a NoteOn message for this velocity if not ready yet.
        // Capture the first ever peak detected by the piezo-sensor and ignore
        // the rest
        //
        if (!mNoteOnSent)
        {
            mStatus = NOTE_ON;
            mVelocity = velocity;
            mReadyForMidi = true;
        }
        else
        {
            mReadyForMidi = false;
        }
        
    }
    //
    // Velocity is within the OFF range. Send a NOTE OFF message corresponding to
    // the most recent NOTE ON message
    //
    else
    {
        //
        // Every NOTE ON message requires its corresponding NOTE OFF message, 
        // otherwise the note will play forever.
        // https://www.cs.cmu.edu/~music/cmsip/readings/MIDI%20tutorial%20for%20programmers.html
        //
        if (mNoteOnSent)
        {
            mStatus = NOTE_OFF;
            mVelocity = 0;
            mReadyForMidi = true;
        }
        else
        {
            mReadyForMidi = false;
        }
    }
}

/**
 * @brief Send master a message containing the current status and velocity of this key
 * 
 * The status byte, indicating whether this key has a new midi message is stored at
 * index 0 in the buffer.
 * 
 * The current status will be stored at index 0 in the buffer.
 * The current velocity will be stored at index 1 in the buffer.
 * 
 * @param slave The slave that this key belongs to. The slave will communicate with master over SPI
 * @param bufferSize The buffer size used to craft message for communicating with master
 * @param queueSize The queue size windowing the communication of the slave wit h master
 */
void Key::SendMessageToMaster(ESP32SPISlave *const slave, const size_t bufferSize, const size_t queueSize)
{
    uint8_t message[bufferSize] {(uint8_t) mReadyForMidi, mStatus, mVelocity};
    slave->queue(message, NULL, bufferSize);
    slave->wait();

    //
    // if there is currently no transaction in flight b/n slave and master
    // and all results have been handled, queue new transactions
    //
    if (slave->hasTransactionsCompletedAndAllResultsHandled())
    {
        slave->queue(message, NULL, bufferSize);
        slave->trigger();
    }
    //
    // if all transactions are complete and all results (from master) are
    // ready, handle results
    //
    if (slave->hasTransactionsCompletedAndAllResultsReady(queueSize))
    {
        const std::vector<size_t> receivedBytes = slave->numBytesReceivedAll();
    } 
}