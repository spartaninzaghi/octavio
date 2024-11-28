/**
 * @file Key.cpp
 * @author Mate Narh
 */

#include "Key.h"

/**
 * @brief Constructor
 * @param notePin The ADC pin that reads the velocity of this key's note/pitch
 * @param damperPin The digital input pin that reads the status of this key's damper
 * @param note The note that this key sounds (0-127)
 * @param maxAdcValue The maximum ADC value recorded by the ADC pin of this key
 * @param thershold The trigger point that evokes a NOTE ON status for this key (range: 0 - 127)
 */
Key::Key(const int notePin, const int damperPin, const uint8_t note, const int maxAdcValue, const int threshold) : 
    mNotePin(notePin), mDamperPin(damperPin), mNote(note), mMaxAdcValue(maxAdcValue), mThreshold(threshold) {}

/**
 * @brief Set the ADC pin that reads the note velocity values of this key
 * @param notePin The ADC pin to set
 */
void Key::SetNotePin(const int pin)
{
    mNotePin = pin;
}

/**
 * @brief Set the digital input pin that reads the status of this key's damper
 * @param damperPin The digital input pin that reads the status of this key's damper
 */
void Key::SetDamperPin(const int damperPin)
{
    mDamperPin = damperPin;
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
 * @brief Set the threshold beyond which triggers a NOTE ON message for this key
 * @param threshold The NOTE ON-triggering threshold to set
 */
void Key::SetThreshold(uint8_t threshold)
{
    mThreshold = threshold;
}

/**
 * @brief Return the ADC pin that reads the note velocity of this key
 */
int Key::GetNotePin()
{
    return mNotePin;
}


/**
 * @brief Return the digital GPIO pin that determines the damper status for this key
 */
int Key::GetDamperPin()
{
    return mDamperPin;
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
 * @brief Get the NOTE ON threshold trigger for this key
 */
uint8_t Key::GetThreshold()
{
    return mThreshold;
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
    int value = analogRead(mNotePin);
    // Serial.println(value);
    value = map(value, 0, mMaxAdcValue, 0, 127);
    
    uint8_t velocity = constrain(value, 0, 127);
    bool damperIsOn = !digitalRead(mDamperPin); // damper is connected to hall effect sensor that uses active low logic. Therefore LOW -> Damper On ...

    // Serial.println(velocity);
    Serial.print(" | Note: "); Serial.print(mNote); 


    switch(mState)
    {
        case Idle:
            Serial.print(" | State: Idle");
            //
            // While idle, if the registered velocity overshoots the threshold required 
            // to turn on the note of this key, and the damper is off ("lifted"), update 
            // variables, inputs, and outputs. Then, transition to the NoteOn state
            //
            if (!mNoteIsOn && velocity > mThreshold && !damperIsOn)
            {
                mVelocity = velocity;
                mStatus = NOTE_ON;
                mNoteIsOn = true;
                mReadyForMIDI = true;

                mState = NoteOn;

                Serial.print(" | Velocity: "); Serial.print(velocity);
            }
            //
            // Otherwise, remain in this Idle state
            //
            else
            {
                mReadyForMIDI = false;
            }
            break;

        case NoteOn:
            Serial.print(" | State: NoteOn");
            //
            // While the note of this key is on, if the damper turns on, signifying that the
            // bottom of the key has returned to its rest state, update variables, inputs, & 
            // outputs. Then, transition to the NoteOff state
            //
            if (mNoteIsOn && velocity < mThreshold &&damperIsOn)
            {
                mVelocity = 0;
                mStatus = NOTE_OFF;
                mNoteIsOn = false;
                mReadyForMIDI = true;

                mState = NoteOff;
                Serial.print(" | Velocity: "); Serial.print(velocity);
            }
            //
            // Otherwise, if a note is currently on, but the key is pressed again, and the 
            // resulting velocity overshoots the threshold without the damper turning on 
            // (muting the string), update variables and transition to the PartialPressNoteOn 
            // state. During this transition, send a NOTE OFF message to quench the penultimate 
            // note before sending the new NOTE ON message at the newly registered velocity. 
            // This is important because, "Every NOTE ON message requires its corresponding NOTE
            // OFF message, otherwise the note will play forever"
            // see: https://www.cs.cmu.edu/~music/cmsip/readings/MIDI%20tutorial%20for%20programmers.html
            //
            else if (mNoteIsOn && velocity > mThreshold &&  !damperIsOn)
            {
                temp = velocity;
                mVelocity = 0;
                mStatus = NOTE_OFF;
                mNoteIsOn = false;
                mReadyForMIDI = true;

                mState = PartialPressNoteOn;
                Serial.print(" | Velocity: "); Serial.print(velocity);
            }
            //
            // Else, finally, remain in this NOTE ON state. Why? The user might stil
            // be holding down a key, causing the damper to remain lifted
            //
            else
            {
                mReadyForMIDI = false;
            }
            break;

        case PartialPressNoteOn:
            Serial.print(" | State: PartialPressNoteOn");
            Serial.print(" | Velocity: "); Serial.print(mVelocity);
            //
            // Entering this state implies that although the bottom of the key did not return
            // to its rest position - which would have caused the damper to turn on and mute 
            // the string - the key was pressed again, evoking a new NOTE ON message. 
            //
            // However, in order to play the new NOTE ON message, during the transition, a
            // NOTE OFF message was sent to quench the penultimate note
            //
            mVelocity = temp;
            mStatus = NOTE_ON;
            mNoteIsOn = true;
            mReadyForMIDI = true;

            mState = NoteOn;

            break;
        
        case NoteOff:
            Serial.print("State: NoteOff");
            Serial.print(" | Velocity: "); Serial.print(mVelocity);
            //
            // From the NoteOff state transition directly to the Idle state
            //
            mReadyForMIDI = false;
            mState = Idle;
            break;

        default:
            //
            // To avoid fall-through behaviour, transition to the Idle state by default
            // as it is the initial state of the FSM
            //
            mReadyForMIDI = false;
            mState = Idle;
            break;
    }
    Serial.print(" | Damper: "); Serial.print(damperIsOn);
    Serial.println();
}

/**
 * @brief Read and smooth the ADC value of this key's pin using EMA
 * 
 * This function analog reads the value of the given ADC pin and  digitally smoothens
 * out high frequency using an Exponential Moving Average low-pass filter. This allows
 * the desired frequencies (lower than the cutoff frequency) to pass through unattenuated,
 * and higher frequencies to be cut off
 * @param pin The pin to analog read and digitally filter
 * @return The smoothed analog value
 */
int Key::analogReadSmoothedWithEMA(const int pin)
{
    //
    // General formula:
    // y = (1 − α)x + αy
    // 
    // where
    //     y = output
    //     x = input
    //     α = smoothing factor (between 0 - 1 | lower values correspond to smoother attenuation)
    //     
    // Credits:
    // https://electronics.stackexchange.com/questions/176721/how-to-smooth-analog-data
    // https://www.luisllamas.es/en/arduino-exponential-low-pass/
    //      

    int rawAnalogValue = analogRead(mNotePin);

    mSmoothedAnalogValue = mSmoothingFactor * rawAnalogValue + (1 - mSmoothingFactor) * mSmoothedAnalogValue;
    
    return static_cast<int>(mSmoothedAnalogValue);
}
