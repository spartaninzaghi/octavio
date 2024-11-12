/**
 * @file Key.h
 * @author Mate Narh
 * 
 * Class for a key on the MIDI Keyboard
 */

#ifndef KEY_H
#define KEY_H

#include <Utility.h>

class Key
{
private:

    /// The analog pin that this key is connected to
    int mPin = 0;                    

    uint8_t mStatus = NOTE_OFF;  ///< The status of this note
    uint8_t mNote = 0;           ///< The note that this key sounds (0 - 127)
    uint8_t mVelocity = 0;       ///< The velocity this key is being played at
    
    int mMaxAdcValue = 12;       ///< The max value read on the ADC pin of this key
    uint8_t mThresholdOn  = 65;  ///< The 1st velocity above this threshold evokes a NOTE ON message
    uint8_t mThresholdOff = 55;  ///< The 1st velocity below this threshold evokes a NOTE OFF message 

    bool mReadyForMIDI = false;  ///< This note is ready to send a MIDI message
    bool mNoteIsOn = false;    ///< Has the velocity of this key been sent

    enum State {Idle, NoteOn, NoteOff};
    State mState = Idle;

    /// At what time was the NOTE ON velocity for this key first recorded ?
    unsigned long mNoteOnTimestamp = 0;

    /// The zero point for this key, taking DC offset into account
    int mBaseline = 59;  

    /// The time taken for the piezo disc of this key to debounce in milliseconds
    const unsigned long mDebounceTime = 500;

    /// Smoothing factor to digitally filter analog data using Exponential Moving Average (EMA) LPF
    const float mSmoothingFactor = 0.6;

    /// The smoothed ADC value
    float mSmoothedAnalogValue = 0.0;


public:

    Key(const int pin, const uint8_t note, const int maxAdcValue, const int baseline, const int thresholdOn, const int thresholdOff);
    ~Key() {};


    // ----------------------------------- Setters ---------------------------------
    void SetPin(int pin);
    void SetStatus(uint8_t status);
    void SetVelocity(uint8_t velocity);
    void SetThresholdOn(uint8_t thresholdOn);
    void SetThresholdOff(uint8_t thresholdOff);

    // ----------------------------------- Getters ---------------------------------
    int GetPin();
    uint8_t GetNote();
    uint8_t GetStatus();
    uint8_t GetVelocity();
    uint8_t GetThresholdOn();
    uint8_t GetThresholdOff();
    bool IsReadyForMIDI();

    // --------------------------------- Core Methods ------------------------------
    void Update();
    uint8_t ScaleVelocity(const int velocity);
    int analogReadSmoothedWithEMA(const int pin);

    Key() = default;
    Key(const Key &) = delete;
    void operator=(const Key &) = delete;
};

#endif