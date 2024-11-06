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
    uint8_t mThreshold = 20;     ///< Cutoff to discriminate b/n presses and noise

    bool mReadyForMIDI = false;  ///< This note is ready to send a MIDI message
    bool mNoteOnSent = false;    ///< Has the velocity of this key been sent
    bool mNoteOffSent = false;    ///< Has a NOTE OFF message been sent to quench the playing note of this key?


public:

    Key(const int pin, const uint8_t note, const int maxAdcValue, const int threshold);
    ~Key() {};


    // ----------------------------------- Setters ---------------------------------
    void SetPin(int pin);
    void SetStatus(uint8_t status);
    void SetVelocity(uint8_t velocity);
    void SetNoteOnSent(bool sent);
    void SetNoteOffSent(bool sent);
    void SetThreshold(uint8_t threshold);

    // ----------------------------------- Getters ---------------------------------
    int GetPin();
    uint8_t GetNote();
    uint8_t GetStatus();
    uint8_t GetVelocity();
    uint8_t* GetCurrentMessageForMaster();
    bool GetNoteOnSent();
    bool GetNoteOffSent();
    bool IsReadyForMIDI();

    // --------------------------------- Core Methods ------------------------------
    void Update();

    Key() = default;
    Key(const Key &) = delete;
    void operator=(const Key &) = delete;
};

#endif