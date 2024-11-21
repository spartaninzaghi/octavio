/**
 * @file RotaryEncoder.cpp
 * @author Mate Narh
 */

#include <Arduino.h>
#include "RotaryEncoder.h"


/**
 * @brief Constructor
 * @param clockPin The GPIO pin for this rotary encoder's clock signal
 * @param dataPin The GPIO pin for this rotary encoder's data signal
 * @param switchPin The GPIO pin for this rotarty encoder's switch
 * @param counterMax The ceiling for this rotary encoder's counter
 * @param counterMin The floor for this rotary encoder's counter
 */
RotaryEncoder::RotaryEncoder(const int clockPin, const int dataPin, const int switchPin, const int counterMax, const int counterMin) :
    mClockPin(clockPin), mDataPin(dataPin), mSwitchPin(switchPin), mCounterMax(counterMax), mCounterMin(counterMin) 
{
    //
    // Initialize previous clock and switch state to values at setup
    //
    mPreviousClockState = digitalRead(clockPin);
    mPreviousSwitchState = digitalRead(switchPin);
}

/**
 * @brief Destructor
 */
RotaryEncoder::~RotaryEncoder()
{
}

/**
 * @brief Set the ceiling for this rotary encoder's counter
 * @param counterMax The new ceiling to set
 */
void RotaryEncoder::SetCounterMax(int counterMax)
{
    mCounterMax = counterMax;
}

/**
 * @brief Set the floor for this rotary encoder's counter
 * @param counterMax The new floor to set
 */
void RotaryEncoder::SetCounterMin(int counterMin)
{
    mCounterMin = counterMin;
}

/**
 * @brief Set the callback handler defining what happens when the switch of this rotary encoder is pressed
 * @param callback The callback handler for a switch pressed event
 */
void RotaryEncoder::SetSwitchCallback(void(*callback)())
{
    
}

/**
 * @brief Update the state of this rotary encoder
 * 
 * Read the digital values of the DATA, CLOCK, and SWITCH signals for this
 * rotary encoder, and update their states accordingly. Also update the 
 * counter to increment by a unit for clockwise rotation and decrement by
 * a unit for counter-clockwise rotation
 */
void RotaryEncoder::Update()
{
    unsigned long currentTime = millis();

    int currentClockState = digitalRead(mClockPin);
    int currentDataState = digitalRead(mDataPin);
    int currentSwitchState = digitalRead(mSwitchPin);

    bool clockStateChanged = currentClockState != mPreviousClockState;

    //
    // --------------------------- Handle Rotations --------------------------
    //
    // When a state change occurs, react only on the rising edge of the
    // clock to avoid double counting. Recall that the state change will
    // also be apparent with the falling edge, but only one event suffices
    //
    if (clockStateChanged && currentClockState == 1 && currentTime - mRotationTimestamp > mRotationDebounceTime)
    {
        //
        // Clockwise Rotation
        //
        // If the current state of the DATA pin does not equal the current
        // CLOCK state, then the CLOCK signal is leading the DATA signal by
        // a phase shift of 90 degrees, implying clockwise rotation
        //
        //         ______     ______     ______        
        // Clock:  |    |_____|    |_____|    |_____   
        //            ______     ______     ______     
        // Data :  ___|    |_____|    |_____|    |_____
        //
        if (currentClockState != digitalRead(mDataPin))
        {
            mCounter = constrain((mCounter + 1), mCounterMin, mCounterMax);
            mDirection = Clockwise;
        }   

        //
        // Counter-clockwise Rotation
        //
        // If the current state of the DATA pin equals the current CLOCK state
        // then the CLOCK signal is lagging behind the DATA signal by a phase
        // shift of 90 degrees, implying counter-clockwise rotation
        //
        //            ______     ______     ______     
        // Clock:  ___|    |_____|    |_____|    |_____
        //         ______     ______     ______        
        // Data :  |    |_____|    |_____|    |_____   
        //
        else
        {
            mCounter = constrain((mCounter - 1), mCounterMin, mCounterMax);
            mDirection = CounterClockwise;
        }

        mRotationTimestamp = currentTime;        
    }
    //
    // Update the previous clock state
    //
    mPreviousClockState = currentClockState;


    //
    // -------------------------- Handle Switch Event -----------------------
    //
    bool switchStateChanged = currentSwitchState != mPreviousSwitchState;

    //
    // The switch is connected to a GPIO pin configured as an INPUT PULLUP pin
    // Hence, it uses active LOW logic. Therefore when the switch state is LOW
    // it is ON, and when it is HIGH, it is OFF
    //
    
    bool switchIsOn = switchStateChanged && currentSwitchState == LOW;

    if (switchIsOn && currentTime - mSwitchTimestamp > mSwitchDebounceTime)
    {
        //
        // If there is a switch callback handler for this rotary encoder, call it
        //
        if (mSwitchCallback != nullptr)
        {
            mSwitchCallback();
        }
        else
        {
            HandleSwitchPressedEvent();
        } 

        mSwitchTimestamp = currentTime;
    }
}

/**
 * @brief Reset the value of this rotary encoder's counter to 0
 */
void RotaryEncoder::ResetCounter()
{
    mCounter = 0;
}

/**
 * @brief Execute action when the switch of this rotary encoder is pressed
 */
void RotaryEncoder::HandleSwitchPressedEvent()
{
    ResetCounter();
}

/**
 * @brief Get the current counter value for this rotary encoder
 * @return The current counter vlaue for this rotary encoder
 */
int RotaryEncoder::GetCounter() const
{
    return mCounter;
}


/**
 * @brief Get the maximum counter value possible for this rotary encoder
 * @return The maximum counter value possible for this rotary encoder
 */
int RotaryEncoder::GetCounterMax() const
{
    return mCounterMax;
}

/**
 * @brief Get the minimum counter value possible for this rotary encoder
 * @return The minimum counter value possible for this rotary encoder
 */
int RotaryEncoder::GetCounterMin() const
{
    return mCounterMin;
}

/**
 * @brief Get the most recent direction of rotation for this encoder
 * @return True if this rotary encoder is rotating in the clockwise direction and false otherwise
 */
bool RotaryEncoder::GetDirection() const
{
    return (mDirection == Clockwise);
}



