#include "PitchWheel.h"
/**
 * @file PitchWheel.cpp
 * @author Mate Narh
 */


/**
 * @brief Constructor
 * @param mPitchBendPin The ADC pin for reading this pitch wheel's pitch bend values
 * @param minBend The minimum bend readable by the analog sensor for this pitch wheel
 * @param maxBend The maximum bend readable by the analog sensor for this pitch wheel
 */
PitchWheel::PitchWheel(const int pitchBendPin, const int16_t minBend, const int16_t maxBend) : 
    mPitchBendPin(pitchBendPin), mMinBend(minBend), mMaxBend(maxBend)
{
    //
    // Average 100 samples to callibrate the analog joystick module and
    // set the minimum and maximum bend values
    //
    int16_t sample = 0;

    for (int i = 0; i < mSampleSize; i++)
    {
        sample += analogRead(mPitchBendPin);
    }

    int16_t averageBend = sample / mSampleSize;

    Serial.print("Callibrated average bend: "); Serial.print(averageBend);
    Serial.println();

    // mDeadzoneMin = averageBend - mHysteresis;
    // mDeadzoneMax = averageBend + mHysteresis;
}

/**
 * @brief Destructor
 */
PitchWheel::~PitchWheel()
{

}

/**
 * @brief Get the current bend value of this pitch wheel
 */
int16_t PitchWheel::GetBend() const
{
    return mBend;
}

/**
 * @brief Get the minimum bend readable by this pitch wheel's analog sensors
 * @return The minimum bend readable by this pitch wheel's analog sensors
 */
int16_t PitchWheel::GetMinBend() const
{
    return mMinBend;
}

/**
 * @brief Get the maximum bend readable by this pitch wheel's analog sensors
 * @return The maximum bend readable by this pitch wheel's analog sensors
 */
int16_t PitchWheel::GetMaxBend() const
{
    return mMaxBend;
}

/**
 * @brief Get the deadzone floor for this pitch wheel
 * @return The deadzone floor for this pitch wheel
 */
int16_t PitchWheel::GetDeadzoneMin() const
{
    return mDeadzoneMin;
}

/**
 * @brief Get the deadzone ceiling for this pitch wheel
 * @return The deadzone ceiling for this pitch wheel
 */
int16_t PitchWheel::GetDeadzoneMax() const
{
    return mDeadzoneMax;
}

/**
 * @brief Set the minimum bend value read by this pitch wheel's analog sensors
 * @param bendMin The new minimum bend value to set
 */
void PitchWheel::SetMinBend(const int16_t bendMin)
{
    mMinBend = bendMin;
}

/**
 * @brief Se the maximum bend value readable by this pitch wheel's analog sensors
 * @param bendMax The new bend deazone floor to set
 */
void PitchWheel::SetMaxBend(const int16_t bendMax)
{
    mMaxBend = bendMax;
}

/**
 * @brief Set the minimum threshold for the deadzone
 * @param deadzoneMin The new deadzone floor to set
 */
void PitchWheel::SetDeadzoneMin(const int16_t deadzoneMin)
{
    mDeadzoneMin = deadzoneMin;
}

/**
 * @brief Set the maximum threshold for the deadzone
 * @param deadzoneMax Thhe new deadzone ceiling to set
 */
void PitchWheel::SetDeadzoneMax(const int16_t deadzoneMax)
{
    mDeadzoneMax = deadzoneMax;
}

/**
 * @brief Update the pitch bend value for this pitch wheel
 */
void PitchWheel::Update()
{
    int16_t value = analogRead(mPitchBendPin);

    mBend = (value < mDeadzoneMin) ? map(value, 0, mDeadzoneMin, mMinBend, 0) :
            (value > mDeadzoneMax) ? map(value, mDeadzoneMax, 4095, 0, mMaxBend) : 0; 
}
