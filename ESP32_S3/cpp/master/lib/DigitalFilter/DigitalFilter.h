/**
 * @file DigitalFilter.h
 * @author Mate Narh
 *
 * This class implements a digital filter for attenuating noise using an
 * Exponential Moving Average. This can be used to implement a low pass
 * filter or high pass filter depending on the smoothing factor.
 * 
 * General formula:
 * y = (1 − α)x + αy
 * 
 * where
 *     y = output
 *     x = input
 *     α = smoothing factor (between 0 - 1 | lower values correspond to smoother attenuation)
 *     
 * Credits:
 * https://electronics.stackexchange.com/questions/176721/how-to-smooth-analog-data
 * https://www.luisllamas.es/en/arduino-exponential-low-pass/
 */

class DigitalFilter
{
    private:
        /// @brief Smoothing factor b/n 0 and 1 for digital filtering. Lower values correspond to more smoothening.
        float mSmoothingFactor = 0.01; 

        /// @brief The tracker
        float mSmoothedAnalogValue = 0.0;

    public:
        DigitalFilter(float smoothingFactor);
        ~DigitalFilter();
       
        // ----------------------------------- Setters ------------------------------------
        void SetSmoothingFactor(float smoothingFactor);

        // ----------------------------------- Getters ------------------------------------
        float GetSmoothingFactor() const;
        
        // --------------------------------- Core Methods ---------------------------------
        int analogReadSmoothedWithEMA(const int pin);


        DigitalFilter() = delete;                       ///< Default constructor disabled
        DigitalFilter(const DigitalFilter &) = delete;     ///< Copy constructor disabled
        void operator=(const DigitalFilter &) = delete; ///< Assignment operator disabled
};