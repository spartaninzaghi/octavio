#include "PitchWheel.h"
/**
 * @file PitchWheel.cpp
 * @author Mate Narh
 */

#include <Arduino.h>

/**
 * @brief Constructor
 * @param minBend The minimum bend readable by the analog sensor for this pitch wheel
 * @param maxBend The maximum bend readable by the analog sensor for this pitch wheel
 */
PitchWheel::PitchWheel(const int minBend, const int maxBend) : mMinBend(minBend), mMaxBend(maxBend)
{
    //
    // Average 20 samples to callibrate the analog joystick module and
    // set the minimum and maximum bend values
    //
    int sample = 0;

    for (int i = 0; i < 20; i++)
    {
        sample += analogRead(mPin);
    }

    int averageBend = sample / 20;

    mMinBend = averageBend - mHysteresis;
    mMaxBend = averageBend + mHysteresis;
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
int PitchWheel::GetBend() const
{
    return mBend;
}

/**
 * @brief Get the minimum bend readable by this pitch wheel's analog sensors
 * @return The minimum bend readable by this pitch wheel's analog sensors
 */
int PitchWheel::GetMinBend() const
{
    return mMinBend;
}

/**
 * @brief Get the maximum bend readable by this pitch wheel's analog sensors
 * @return The maximum bend readable by this pitch wheel's analog sensors
 */
int PitchWheel::GetMaxBend() const
{
    return mMaxBend;
}

/**
 * @brief Get the deadzone floor for this pitch wheel
 * @return The deadzone floor for this pitch wheel
 */
int PitchWheel::GetDeadzoneMin() const
{
    return mDeadzoneMin;
}

/**
 * @brief Get the deadzone ceiling for this pitch wheel
 * @return The deadzone ceiling for this pitch wheel
 */
int PitchWheel::GetDeadzoneMax() const
{
    return mDeadzoneMax;
}

/**
 * @brief Set the minimum bend value read by this pitch wheel's analog sensors
 * @param bendMin The new minimum bend value to set
 */
void PitchWheel::SetMinBend(const int bendMin)
{
    mMinBend = bendMin;
}

/**
 * @brief Se the maximum bend value readable by this pitch wheel's analog sensors
 * @param bendMax The new bend deazone floor to set
 */
void PitchWheel::SetMaxBend(const int bendMax)
{
    mMaxBend = bendMax;
}

/**
 * @brief Set the minimum threshold for the deadzone
 * @param deadzoneMin The new deadzone floor to set
 */
void PitchWheel::SetDeadzoneMin(const int deadzoneMin)
{
    mDeadzoneMin = deadzoneMin;
}

/**
 * @brief Set the maximum threshold for the deadzone
 * @param deadzoneMax Thhe new deadzone ceiling to set
 */
void PitchWheel::SetDeadzoneMax(const int deadzoneMax)
{
    mDeadzoneMax = deadzoneMax;
}

/**
 * @brief Update the pitch bend value for this pitch wheel
 */
void PitchWheel::Update()
{
    int value = analogRead(mPin);

    mBend = (value < mDeadzoneMin) ? map(value, 0, mDeadzoneMin, mMinBend, mDeadzoneMin) :
            (value > mDeadzoneMax) ? map(value, mDeadzoneMax, 4095, 0, mMaxBend) : 0; 
}
