/**
 * @file Key.cpp
 * @author Mate Narh
 */

#include "Key.h"


/**
 * @brief Constructor
 * @param pin The GPIO pin bound to this key
 * @param note The note that this key sounds (0-127)
 */
Key::Key(const int pin, const uint8_t note, const int maxAdcValue, const int baseline, const int thresholdOn, const int thresholdOff) : 
    mPin(pin), mNote(note), mMaxAdcValue(maxAdcValue), mBaseline(baseline), mThresholdOn(thresholdOn), mThresholdOff(thresholdOff) {}

/**
 * @brief Bind this key to specified GPIO pin
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
 * @brief Set the threshold beyond which triggers a NOTE ON message for this key
 * @param thresholdOn The NOTE ON-triggering threshold to set
 */
void Key::SetThresholdOn(uint8_t thresholdOn)
{
    mThresholdOn = thresholdOn;
}

/**
 * @brief Set the threshold below which triggers a NOTE OFF message for this key
 * @param thresholdOff The NOTE OFF-triggering threshold to set
 */
void Key::SetThresholdOff(uint8_t thresholdOff)
{
    mThresholdOff = thresholdOff;
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
 * @brief Get the NOTE ON threshold trigger for this key
 */
uint8_t Key::GetThresholdOn()
{
    return mThresholdOn;
}

/**
 * @brief Get the NOTE OFF threshold trigger for this key
 */
uint8_t Key::GetThresholdOff()
{
    return mThresholdOff;
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
    int value = analogReadSmoothedWithEMA(mPin);
    value = map(value, 0, 4095, 0, 127);
    
    uint8_t velocity = constrain(value, 0, 127);

    Serial.println(velocity);

    switch(mState)
    {
        case Idle:
            //
            // While idle, if the registered velocity overshoots the threshold required 
            // to turn on the note of this key, but its note is currently off, update 
            // variables, inputs, and outputs. Then, transition to the NoteOn state
            //
            if (velocity > mThresholdOn && !mNoteIsOn)
            {
                mVelocity = velocity;
                mStatus = NOTE_ON;
                mNoteIsOn = true;
                mReadyForMIDI = true;

                mNoteOnTimestamp = millis();

                mState = NoteOn;
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
            //
            // While the note of this key is on, if the newly registered velocity goes 
            // below the minimum threshold demarcating a NOTE OFF, but the note is 
            // currently on, update variables, inputs, and outputs. Then, transition to
            // the NoteOn state
            //
            if (velocity < mThresholdOff && mNoteIsOn)
            {
                mVelocity = 0;
                mStatus = NOTE_OFF;
                mNoteIsOn = false;
                mReadyForMIDI = true;

                mState = NoteOff;
            }
            else
            {
                unsigned long elapsed = millis() - mNoteOnTimestamp;
                bool newPeakDetected = velocity > mThresholdOn;

                //
                // Otherwise, if a note is currently on, but it is within a deadzone and
                // the maximum debounce time elapses, quench the note and transition it to 
                // the NOTE OFF state. This is because, "Every NOTE ON message requires its 
                // corresponding NOTE OFF message, otherwise the note will play forever"
                // see: https://www.cs.cmu.edu/~music/cmsip/readings/MIDI%20tutorial%20for%20programmers.html
                //
                if (elapsed > mDebounceTime && mNoteIsOn && newPeakDetected)
                {
                    mVelocity = 0;
                    mStatus = NOTE_OFF;
                    mNoteIsOn = false;
                    mReadyForMIDI = true;

                    mState = NoteOff;
                }
                //
                // Else, finally, remain in this NOTE ON state. Why? The user might stil
                // be holding down a key
                //
                else
                {
                    mReadyForMIDI = false;
                }
            }
            break;
        
        case NoteOff:
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
}

/**
 * @brief Return the velocity of this key scaled to 0 - 127, taking DC offset into consideration
 */
uint8_t Key::ScaleVelocity(const int velocity)
{
    if (!velocity)
    {
        return 0;
    }

    uint8_t changeInVelocity = velocity - mBaseline;
    uint8_t maxPossibleChangeInVelocity = 100 - mBaseline;

    uint8_t scaledVelocity = static_cast<uint8_t>( (changeInVelocity/maxPossibleChangeInVelocity) * 127 );

    return scaledVelocity; 
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

    int rawAnalogValue = analogRead(mPin);

    mSmoothedAnalogValue = mSmoothingFactor * rawAnalogValue + (1 - mSmoothingFactor) * mSmoothedAnalogValue;
    
    return static_cast<int>(mSmoothedAnalogValue);
}


