/**
 * @file Key.cpp
 * @author Mate Narh
 * 
 * --Tentative Key Lib
 */

#include "Key.h"

uint8_t message[4] {0, 0, 0, 0};

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

    switch(mState)
    {
        case Idle:
            //
            // If the current velocity exceeds the thresholds but has
            // not been sent, transition to th ReadyForNoteOn state
            //
            if (velocity > mThreshold && !mNoteOnSent)
            {
                mState = ReadyForNoteOn;
            }
            
            //
            // If the current velocity is at or lower than the threshold
            // transition to the ReadyForNoteOff state
            //
            if (velocity <= mThreshold && mNoteOnSent)
            {
                mState = ReadyForNoteOff;
            }

            mReadinessByte = 0x00;
            break;

        case ReadyForNoteOn:
            mStatus = NOTE_ON;
            mVelocity = velocity;
            mNoteOnSent = true;

            mState = Idle;

            Serial.println("Current state: ReadyForNoteOn");


            // Debug lines
            mStatus = NOTE_ON;
            mVelocity = velocity;
            mReadyForMidi = true;
            mNoteOnSent = true;
            mReadinessByte = 0x01;
            Serial.print("Updating key: ");
            Serial.print(mPin);
            Serial.print(" | Current Velocity: ");
            Serial.print(mVelocity);
            Serial.print(" | Threshold: ");
            Serial.print(mThreshold);
            Serial.print(" | Time (s): ");
            Serial.print(millis()/1000.0);
            Serial.println();
            break;

        case ReadyForNoteOff:
            mStatus = NOTE_OFF;
            mVelocity = 0;
            mNoteOnSent = false;
            mReadinessByte = 0x01;

            mState = Idle;

            Serial.println("Current state: ReadyForNoteOff");

            // Debug lines
            Serial.print("NOTE OFF >> Current mVelocity: ");
            Serial.print(mVelocity);
            Serial.print(" | Current Velocity: ");
            Serial.print(velocity);
            Serial.print(" | Threshold: ");
            Serial.print("Sending NOTE OFF @ Time (s): ");
            Serial.print(millis()/1000.0);
            Serial.println();
            break;
    }
}

uint8_t *Key::GetCurrentMessageForMaster()
{
    message[0] = mReadinessByte;
    message[1] = mStatus;
    message[2] = mVelocity;
    message[3] = 0x00; // Don't care, but fill the last block of the buffer

    return message;
}

