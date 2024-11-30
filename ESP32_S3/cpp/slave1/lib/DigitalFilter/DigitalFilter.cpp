/**
 * @file DigitalFilter.cpp
 * @author Mate Narh
 */

#include <Arduino.h>
#include "DigitalFilter.h"

/**
 * @brief Constructor
 * @param smoothingFactor The smoothing factor indicative of the level of filtering that this digital filter applies
 */
DigitalFilter::DigitalFilter(float smoothingFactor) : mSmoothingFactor(smoothingFactor)
{
}

/**
 * @brief Destructor
 */
DigitalFilter::~DigitalFilter()
{

}

/**
 * @brief Set the smoothing factor for this digital filter
 * @param smoothingFactor The smoothing factor to set
 */
void DigitalFilter::SetSmoothingFactor(float smoothingFactor)
{
    mSmoothingFactor = smoothingFactor;
}


float DigitalFilter::GetSmoothingFactor() const
{
    return mSmoothingFactor;
}

/**
 * @brief Read and smooth the ADC value of the given pin using EMA
 * 
 * This function analog reads the value of the given ADC pin and  digitally smoothens
 * out high frequency using an Exponential Moving Average low-pass filter. This allows
 * the desired frequencies (lower than the cutoff frequency) to pass through unattenuated,
 * and higher frequencies to be cut off
 * @param pin The pin to analog read and digitally filter
 * @return The smoothed analog value
 */
int DigitalFilter::analogReadSmoothedWithEMA(const int pin)
{
    int rawAnalogValue = analogRead(pin);
    mSmoothedAnalogValue = mSmoothingFactor * rawAnalogValue + (1 - mSmoothingFactor) * mSmoothedAnalogValue;
    return static_cast<int>(mSmoothedAnalogValue);
}
