/**
 * @file RotaryEncoder.h
 * @author Mate Narh
 * 
 * Logic Driver for a 5-pin rotary encoder module
 * 
 * Pins: GND, VCC, SW, DT, CLK
 * 
 * Signals: 
 *         SW (switch) 
 *         DT (data)   | aka OUTPUT B 
 *         CLK (clock) | aka OUTPUT A
 */

#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

class RotaryEncoder
{
    private:

        const int mDataPin  = 0;       ///< The data pin for this rotary encoder
        const int mClockPin = 0;       ///< The clock pin for this rotary encoder
        const int mSwitchPin = 0;      ///< The switch pin for this rotary encoder
          
        int mPreviousClockState = 0;   ///< The last state of the clock signal
        int mPreviousSwitchState = 0;  ///< The last sate of the switch signal
        
        int mCounter = 0;     ///< The value to decrement or increment in response to rotation
        int mCounterMax = 0;  ///< The counter ceiling for this rotary encoder
        int mCounterMin = 0;  ///< The counter floor for this rotary encoder

        unsigned long mSwitchDebounceTime = 1;    ///< The debounce time for the switch
        unsigned long mRotationDebounceTime = 1;  ///< The debounce time for rotations

        unsigned long mSwitchTimestamp = 0;       ///< When did the last valid switch press take place?
        unsigned long mRotationTimestamp = 0;     ///< When did the last valid rotation take place?

        /// brief The possible directions for this rotary encoder's knob to move in
        enum Direction {Clockwise, CounterClockwise};

        Direction mDirection = Clockwise;     ///< What direction is this rotary encoder currently rotating in?

        void (*mSwitchCallback)() = nullptr;  ///< Callback handler for when the switch of this rotary encoder is pressed

    public:
        RotaryEncoder(const int clockPin, const int dataPin, const int switchPin, const int counterMax, const int counterMin);
        ~RotaryEncoder();

        // ----------------------------------- Setters ------------------------------------
        void SetCounterMax(int counterMax);
        void SetCounterMin(int counterMin);
        void SetSwitchCallback(void(*callback)());

        // ----------------------------------- Getters ------------------------------------
        int GetCounter() const;
        int GetCounterMax() const;
        int GetCounterMin() const;
        bool GetDirection() const;

        // --------------------------------- Core Methods ---------------------------------
        void Update();
        void ResetCounter();
        void HandleSwitchPressedEvent();

        RotaryEncoder() = delete;                       ///< Default constructor disabled
        RotaryEncoder(const RotaryEncoder &) = delete;  ///< Copy constructor disabled
        void operator=(const RotaryEncoder &) = delete; ///< Assignment operator disabled
};

#endif // ROTARY_ENCODER_H