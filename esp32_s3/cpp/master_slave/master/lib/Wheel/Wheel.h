/**
 * @file Wheel.h
 * @author Mate Narh
 * 
 * Class to interface with a joystick module or like sensor to drive MIDI 
 * Pitch Bend messages in application
 */

#ifndef PITCH_WHEEL_H
#define PITCH_WHEEL_H

#include <Arduino.h>
#include <DigitalFilter.h>


class Wheel
{
    private:

        const int mWheelPin = 0;   /// The ADC pin used to poll analog data for this Wheel
        int mMaxAnalogValue = 0;   /// The maximum analog value readable on the ADC pin(s) of this wheel

        int16_t mReading = 0;      ///< The analog reading on the ADC pin of this wheel
        int16_t mPrevReading = 0;  ///< The last analog value read on the ADC pin of this wheel
        
        int mSampleSize = 100;     ///< Number of ADC values to sample for sensor callibration
        
        int16_t mRangeMin = 0;     ///< The minimum pitch bend value scaled from the analog sensor of this pitch wheel
        int16_t mRangeMax = 0;     ///< The maximum pitch bend value scaled from the analog sensor of this pitch wheel
        
        int16_t mDeadzoneMin = 0;  ///< The lower boundary of the deadzone. Values within the deadzone are treated as 0
        int16_t mDeadzoneMax = 0;  ///< The upper boundary of the deadzone. Values within the deadzone are treated as 0

        int16_t mHysteresis = 100; ///< The padding used to account for deadzone at the joystick's center during callibration  

        /// Debounce time to reduce the rate at which messages are sent w.r.t this wheel
        unsigned long mDebounceTime = 10;

        /// Smoothing factor to digitally filter analog data using Exponential Moving Average (EMA) LPF
        const float mSmoothingFactor = 0.1;

        /// The digital EMA filter for smoothening out high frequency noise on the ADC pin of this wheel
        DigitalFilter* mDigitalFilter = nullptr;

        /// Flag indicating whether the bend value for this pitch wheel changed
        bool mReadingChanged = false;


    public:

        Wheel(const int wheelPin, const int resolution, const int16_t rangeMin, const int16_t rangeMax);
        ~Wheel();
        
        // ----------------------------------- Setters ------------------------------------
        int16_t GetReading() const;
        int16_t GetRangeMin() const;
        int16_t GetRangeMax() const;
        int16_t GetDeadzoneMin() const;
        int16_t GetDeadzoneMax() const;
        bool IsReadingChanged() const;

        // ----------------------------------- Getters ------------------------------------
        void SetRangeMin(const int16_t bendMin);
        void SetRangeMax(const int16_t bendMax);
        void SetDeadzoneMin(const int16_t deadzoneMin);
        void SetDeadzoneMax(const int16_t deadzoneMax);
        
        // --------------------------------- Core Methods ---------------------------------
        void Update();
        void Callibrate();


        Wheel() = delete;                       ///< Default constructor disabled
        Wheel(const Wheel &) = delete;          ///< Copy constructor disabled
        void operator=(const Wheel &) = delete; ///< Assignment operator disabled
};

#endif // PITCH_WHEEL_H
