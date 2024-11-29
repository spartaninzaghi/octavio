/**
 * @file PitchWheel.h
 * @author Mate Narh
 * 
 * Class to interface with a joystick module or like sensor to drive MIDI 
 * Pitch Bend messages in application
 */

#ifndef PITCH_WHEEL_H
#define PITCH_WHEEL_H

#include <Arduino.h>

class PitchWheel
{
    private:

        /// The GPIO pin used to poll analog data for this Picth Wheel
        const int mPitchBendPin = 4; 

        /// The bend value for this Pitch Wheel. Ideally [-8192 <-> 0 <-> 8192]
        int16_t mBend = 0;  
        int16_t mPreviousBend = 0;

        /// Accounts for deadzone region at center of analog joystick module
        int16_t mHysteresis = 50;  
        
        int16_t mMinBend = 0;      ///< The minimum pitch bend value scaled from the analog sensor of this pitch wheel
        int16_t mMaxBend = 0;      ///< The maximum pitch bend value scaled from the analog sensor of this pitch wheel

        int16_t mDeadzoneMin = 1800;  ///< The lower boundary of the deadzone. Values within the deadzone are treated as 0
        int16_t mDeadzoneMax = 2100;  ///< The upper boundary of the deadzone. Values within the deadzone are treated as 0

        /// Smoothing factor to digitally filter analog data using Exponential Moving Average (EMA) LPF
        const float mSmoothingFactor = 0.4;

        /// The smoothed ADC value
        float mSmoothedAnalogValue = 0.0;

        ///< Flag indicating whether the bend value for this pitch wheel changed
        bool mBendChanged = false;

        int mSampleSize = 100;      ///< Number of ADC values to sample for sensor callibration

    public:

        PitchWheel(const int pitchBendPin, const int16_t minBend, const int16_t maxBend);
        ~PitchWheel();
        
        // ----------------------------------- Setters ------------------------------------
        int16_t GetBend() const;
        int16_t GetMinBend() const;
        int16_t GetMaxBend() const;
        int16_t GetDeadzoneMin() const;
        int16_t GetDeadzoneMax() const;
        bool IsBendChanged() const;

        // ----------------------------------- Getters ------------------------------------
        void SetMinBend(const int16_t bendMin);
        void SetMaxBend(const int16_t bendMax);
        void SetDeadzoneMin(const int16_t deadzoneMin);
        void SetDeadzoneMax(const int16_t deadzoneMax);
        
        // --------------------------------- Core Methods ---------------------------------
        void Update();
        int analogReadSmoothedWithEMA(const int pin);


        PitchWheel() = delete;                       ///< Default constructor disabled
        PitchWheel(const PitchWheel &) = delete;     ///< Copy constructor disabled
        void operator=(const PitchWheel &) = delete; ///< Assignment operator disabled
};

#endif // PITCH_WHEEL_H
