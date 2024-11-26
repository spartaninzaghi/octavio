/**
 * @file Key.h
 * @author Mate Narh
 * 
 * Class for a key on the MIDI Keyboard
 */

#ifndef KEY_H
#define KEY_H

#include <Arduino.h>
#include <Utility.h>

class Key
{
private:

    /// The ADC pin that triggers the note/pitch of this key
    int mNotePin = 0;

    /// The digital GPIO pin that determines whether this key has returned to a resting state after a press   
    int mDamperPin = 0;                 

    uint8_t mStatus = NOTE_OFF;  ///< The status of this note
    uint8_t mNote = 0;           ///< The note that this key sounds (0 - 127)
    uint8_t mVelocity = 0;       ///< The velocity this key is being played at
    
    int mMaxAdcValue = 12;       ///< The max value read on the ADC pin of this key
    uint8_t mThreshold  = 65;     ///< The 1st velocity above this threshold evokes a NOTE ON message

    bool mReadyForMIDI = false;  ///< This note is ready to send a MIDI message
    bool mNoteIsOn = false;      ///< Has the velocity of this key been sent

    enum State {Idle, NoteOn, PartialPressNoteOn, NoteOff};
    State mState = Idle;

    /// At what time was the NOTE ON velocity for this key first recorded ?
    unsigned long mNoteOnTimestamp = 0;

    /// The time taken for the piezo disc of this key to debounce in milliseconds
    const unsigned long mDebounceTime = 1000;

    /// Smoothing factor to digitally filter analog data using Exponential Moving Average (EMA) LPF
    const float mSmoothingFactor = 0.4;

    /// The smoothed ADC value
    float mSmoothedAnalogValue = 0.0;

    /// @brief  Temporary variable for managing edgy transitions in state machine
    int temp = 0;



public:

    Key(const int pin, const int damperPin, const uint8_t note, const int maxAdcValue, const int threshold);
    ~Key() {};


    // ----------------------------------- Setters ---------------------------------
    void SetNotePin(int notePin);
    void SetDamperPin(int damperPin);
    void SetStatus(uint8_t status);
    void SetVelocity(uint8_t velocity);
    void SetThreshold(uint8_t thresholdOn);

    // ----------------------------------- Getters ---------------------------------
    int GetNotePin();
    int GetDamperPin();
    uint8_t GetNote();
    uint8_t GetStatus();
    uint8_t GetVelocity();
    uint8_t GetThreshold();
    bool IsReadyForMIDI();

    // --------------------------------- Core Methods ------------------------------
    void Update();
    int analogReadSmoothedWithEMA(const int pin);

    Key() = default;
    Key(const Key &) = delete;
    void operator=(const Key &) = delete;
};

#endif // KEY_H