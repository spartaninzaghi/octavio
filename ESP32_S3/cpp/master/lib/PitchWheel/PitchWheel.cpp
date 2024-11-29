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
    int sample = 0;
    int sampleTotal = 0;
    int16_t averageBend = 0;

    for (int i = 0; i < mSampleSize; i++)
    {
        sample = analogRead(mPitchBendPin);
        Serial.println(sample);

        sampleTotal += sample;
    }

    averageBend = sampleTotal / mSampleSize;

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

bool PitchWheel::IsBendChanged() const
{
    return mBendChanged;
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
    int16_t value = analogReadSmoothedWithEMA(mPitchBendPin);

    mBend = (value < mDeadzoneMin) ? map(value, 0, mDeadzoneMin, mMinBend, 0) :
            (value > mDeadzoneMax) ? map(value, mDeadzoneMax, 4095, 0, mMaxBend) : 0; 

    if (mPreviousBend != mBend)
    {
        mBendChanged = true;
        mPreviousBend = mBend;
    }
    else
    {
        mBendChanged = false;
    }
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
int PitchWheel::analogReadSmoothedWithEMA(const int pin)
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

    int rawAnalogValue = analogRead(pin);

    mSmoothedAnalogValue = mSmoothingFactor * rawAnalogValue + (1 - mSmoothingFactor) * mSmoothedAnalogValue;
    
    return static_cast<int>(mSmoothedAnalogValue);
}
