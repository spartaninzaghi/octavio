#include "Wheel.h"
/**
 * @file Wheel.cpp
 * @author Mate Narh
 */


/**
 * @brief Constructor
 * @param mWheelPin The ADC pin for reading this wheel's analog values
 * @param rangeMin The minimum value readable by the analog sensor for this pitch wheel
 * @param rangeMax The maximum value readable by the analog sensor for this pitch wheel
 */
Wheel::Wheel(const int wheelPin, const int resolution, const int16_t rangeMin, const int16_t rangeMax) : 
    mWheelPin(wheelPin), mRangeMin(rangeMin), mRangeMax(rangeMax)
{
    //
    // Set the maximum analog value at which this Wheel's ADC pin(s) will be analog read
    // Bit math for: MAX ADC VALUE = 2^resolution - 1 
    //
    mMaxAnalogValue = (1 << resolution) - 1;

    //
    // Callibrate this wheel
    //
    Callibrate();
    
    //
    // Initialize digital EMA filter to smoothen out ADC noise
    //
    mDigitalFilter = new DigitalFilter(mSmoothingFactor);
}

/**
 * @brief Destructor
 */
Wheel::~Wheel()
{

}

/**
 * @brief Get the current bend value of this pitch wheel
 */
int16_t Wheel::GetReading() const
{
    return mReading;
}

/**
 * @brief Get the minimum bend readable by this pitch wheel's analog sensors
 * @return The minimum bend readable by this pitch wheel's analog sensors
 */
int16_t Wheel::GetRangeMin() const
{
    return mRangeMin;
}

/**
 * @brief Get the maximum bend readable by this pitch wheel's analog sensors
 * @return The maximum bend readable by this pitch wheel's analog sensors
 */
int16_t Wheel::GetRangeMax() const
{
    return mRangeMax;
}

/**
 * @brief Get the deadzone floor for this pitch wheel
 * @return The deadzone floor for this pitch wheel
 */
int16_t Wheel::GetDeadzoneMin() const
{
    return mDeadzoneMin;
}

/**
 * @brief Get the deadzone ceiling for this pitch wheel
 * @return The deadzone ceiling for this pitch wheel
 */
int16_t Wheel::GetDeadzoneMax() const
{
    return mDeadzoneMax;
}

/**
 * @brief Returns whether or not the bend value for this pitch wheel changed
 * @return Flag indicating whether or not the bend value for this pitch wheel changed
 */
bool Wheel::IsReadingChanged() const
{
    return mReadingChanged;
}

/**
 * @brief Set the minimum bend value read by this pitch wheel's analog sensors
 * @param bendMin The new minimum bend value to set
 */
void Wheel::SetRangeMin(const int16_t bendMin)
{
    mRangeMin = bendMin;
}

/**
 * @brief Se the maximum bend value readable by this pitch wheel's analog sensors
 * @param bendMax The new bend deazone floor to set
 */
void Wheel::SetRangeMax(const int16_t bendMax)
{
    mRangeMax = bendMax;
}

/**
 * @brief Set the minimum threshold for the deadzone
 * @param deadzoneMin The new deadzone floor to set
 */
void Wheel::SetDeadzoneMin(const int16_t deadzoneMin)
{
    mDeadzoneMin = deadzoneMin;
}

/**
 * @brief Set the maximum threshold for the deadzone
 * @param deadzoneMax Thhe new deadzone ceiling to set
 */
void Wheel::SetDeadzoneMax(const int16_t deadzoneMax)
{
    mDeadzoneMax = deadzoneMax;
}

/**
 * @brief Update the pitch bend value for this pitch wheel
 */
void Wheel::Update()
{
    int16_t value = mDigitalFilter->analogReadSmoothedWithEMA(mWheelPin);

    mReading = (value < mDeadzoneMin) ? map(value, 0, mDeadzoneMin, mRangeMin, 0) :
               (value > mDeadzoneMax) ? map(value, mDeadzoneMax, mMaxAnalogValue, 0, mRangeMax) : 0; 

    //
    // Set the mReadingChanged flag if the value read for this wheel changed
    //
    if (mPrevReading != mReading)
    {
        mReadingChanged = true;
        mPrevReading = mReading;
    }
    //
    // Clear the mReadingChanged flag otherwise (the current readinge is the same as the last)
    //
    else
    {
        mReadingChanged = false;
    }
}

/**
 * @brief Callibrate this wheel on startup to adapt to transience and noise
 */
void Wheel::Callibrate()
{
    //
    // Average 100 samples to callibrate the analog joystick module and
    // set the minimum and maximum bend values
    //
    int sample = 0;
    int sampleTotal = 0;
    int16_t averageReading = 0;

    for (int i = 0; i < mSampleSize; i++)
    {
        sample = analogRead(mWheelPin);
        Serial.println(sample);

        sampleTotal += sample;
    }

    averageReading = sampleTotal / mSampleSize;

    mDeadzoneMin = averageReading - mHysteresis;
    mDeadzoneMax = averageReading + mHysteresis;

    Serial.println("Callibration Summary");
    Serial.println("---------------------");
    Serial.print("Wheel Pin: "); Serial.print(mWheelPin);
    Serial.print(" | Average Reading: "); Serial.print(averageReading);
    Serial.print(" | Deadzone Min: "); Serial.print(mDeadzoneMin);
    Serial.print(" | Deadzone Max: "); Serial.print(mDeadzoneMax);
    Serial.println(); 
}
