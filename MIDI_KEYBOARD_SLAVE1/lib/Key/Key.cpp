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
    mNoteOnSent = sent;
}

/**
 * @brief Set whether the note of this key has been sent or not
 * @param sent 
 */
void Key::SetNoteOffSent(bool sent)
{
    mNoteOffSent = sent;
}


/**
 * @brief Set the threshold at which this key validates a key press
 * @param threshold The threshold to set
 */
void Key::SetThreshold(uint8_t threshold)
{
    mThreshold = threshold;
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
 * Indicate whether a NOTE ON message has been sent for this key
 */
bool Key::GetNoteOnSent()
{
    return mNoteOnSent;
}

/**
 * Indicate whether a NOTE ON message has been sent for this key
 */
bool Key::GetNoteOffSent()
{
    return mNoteOffSent;
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
bool Key::IsReadyForMIDI()
{
    return mReadyForMIDI;
}

/**
 * @brief Update the status and velocity of this key.
 */
void Key::Update()
{
    int value = analogRead(mPin);
    uint8_t velocity = map(value, 0, 4095, 0, 127); 

    //
    // If the current velocity exceeds the thresholds but has
    // not been sent, transition to th ReadyForNoteOn state
    //
    unsigned long elapsed = millis() - timestamp;

    if (velocity > mThreshold && !mNoteOnSent)
    {
        mVelocity = velocity;
        mStatus = NOTE_ON;
        mReadyForMIDI = true;

        // -------------------- Debug lines ---------------------
        Serial.print("NOTE ON >> Updating key: ");
        Serial.print(mPin);
        Serial.print(" | Current Velocity: ");
        Serial.print(mVelocity);
        Serial.print(" | Threshold: ");
        Serial.print(mThreshold);
        Serial.print(" | Time (s): ");
        Serial.print(millis()/1000.0);
        Serial.println();
    }
    
    //
    // If the current velocity is at or lower than the threshold
    // transition to the ReadyForNoteOff state
    //
    else if (velocity <= mThreshold && mVelocity > mThreshold && mNoteOnSent)
    {
        // mState = ReadyForNoteOff;
        mVelocity = 0;
        mStatus = NOTE_OFF;
        mReadyForMIDI = true;
        mNoteOnSent = false;

        // ---------------------- Debug lines ----------------------
        Serial.print("NOTE OFF >> mVelocity: ");
        Serial.print(mVelocity);
        Serial.print(" | Current Velocity Reading: ");
        Serial.print(velocity);
        Serial.print(" | Threshold: ");
        Serial.print(mThreshold);
        Serial.print(" | Sending NOTE OFF at time (s): ");
        Serial.print(millis()/1000.0);
        Serial.println();
    }
    else
    {
        mReadyForMIDI = false;
    }
}

