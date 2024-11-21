/**
 * @file PitchWheel.h
 * @author Mate Narh
 * 
 * Class to interface with a joystick module or like sensor to drive MIDI 
 * Pitch Bend messages in application
 */

#ifndef PITCH_WHEEL_H
#define PITCH_WHEEL_H

class PitchWheel
{
    private:

        /// The GPIO pin used to poll analog data for this Picth Wheel
        const int mPin = 0; 

        /// The bend value for this Pitch Wheel. Ideally [-8192 <-> 0 <-> 8192]
        int mBend = 0;    

        /// Accounts for deadzone region at center of analog joystick module
        int mHysteresis = 10;  
        
        int mMinBend = 0;      ///< The minimum pitch bend value scaled from the analog sensor of this pitch wheel
        int mMaxBend = 0;      ///< The maximum pitch bend value scaled from the analog sensor of this pitch wheel

        int mDeadzoneMin = 0;  ///< The lower boundary of the deadzone. Values within the deadzone are treated as 0
        int mDeadzoneMax = 0;  ///< The upper boundary of the deadzone. Values within the deadzone are treated as 0


    public:

        PitchWheel(const int minBend, const int maxBend);
        ~PitchWheel();
        
        // ----------------------------------- Setters ------------------------------------
        int GetBend() const;
        int GetMinBend() const;
        int GetMaxBend() const;
        int GetDeadzoneMin() const;
        int GetDeadzoneMax() const;

        // ----------------------------------- Getters ------------------------------------
        void SetMinBend(const int bendMin);
        void SetMaxBend(const int bendMax);
        void SetDeadzoneMin(const int deadzoneMin);
        void SetDeadzoneMax(const int deadzoneMax);
        
        // --------------------------------- Core Methods ---------------------------------
        void Update();


        PitchWheel() = delete;                       ///< Default constructor disabled
        PitchWheel(const PitchWheel &) = delete;  ///< Copy constructor disabled
        void operator=(const PitchWheel &) = delete; ///< Assignment operator disabled
};

#endif // PITCH_WHEEL_H

