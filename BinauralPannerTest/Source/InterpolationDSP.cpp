/*
  ==============================================================================

    InterpolationDSP.cpp
    Created: 9 Apr 2023 6:21:53pm
    Author:  Mitch Glad

  ==============================================================================
*/

#include "InterpolationDSP.h"

InterpolationDSP::InterpolationDSP() : fft (fftOrder)
{
    
    // Reading SOFA file
    bool success = sofa.readSOFAFile("/Users/mitchglad/BinauralPannerTest/BinauralPannerTest/Source/SOFA/SmallTheaterHRIRs_1.0.sofa");
    if(!success)
        return;

}


void InterpolationDSP::getHRIR(int az, int el, float d, juce::AudioBuffer<float> & buffer)
{
    if(d == 2 || d == 6 || d == 10 || d == 14) // If in HRIR database
    {
        for (int c = 0; c < buffer.getNumChannels() ; c++){
            // Getting HRIRs from .sofa file
            const double *hrir = sofa.getHRIR(c, az, el, d);
            
            // Writing this data to a float array
            for(auto n = 0; n < hrirSize; ++n)
            {
                buffer.getWritePointer(c)[n] = static_cast<float> (hrir[n]);
            }
            
        }
    }
    else // If we need to interpolate
    {
        // Finding the mod of our distance
        float dMod = fmod((d-2),4);
        
        // Getting the surrounding distances
        int dLow = d - dMod;
        int dHigh = d + (4 - dMod);
        
        // Finding weights
        const double dLowW = abs((4 - dMod)/4);
        const double dHighW = abs(dMod/4);
            
        for (int c = 0; c < buffer.getNumChannels() ; c++){
            // Getting HRIRs from .sofa file
            const double *hrirLow = sofa.getHRIR(c, az, el, dLow);
            const double *hrirHigh = sofa.getHRIR(c, az, el, dHigh);
            // Writing this data to the bucket HRTFLow
            for(auto n = 0; n < fftSize; ++n)
            {
                HRTFLow[n] = hrirLow[n];
            }
            // FFT Calculation
            fft.performRealOnlyForwardTransform(HRTFLow.data(),true);
            
            // Writing this data to the bucket HRTFHigh
            for(auto n = 0; n < fftSize; ++n)
            {
                HRTFHigh[n] = hrirHigh[n];
            }
            // FFT Calculation
            fft.performRealOnlyForwardTransform(HRTFHigh.data(),true);
            
            // The for loops above and below this are an example of not knowing how many samples to run these for
            
            // Weighting
            for(auto n = 0; n < fftSize * 2; ++n)
            {
                HRTF[n] = (HRTFLow[n] * dLowW) + (HRTFHigh[n] * dHighW);
            }
            
            // IFFT
            fft.performRealOnlyInverseTransform(HRTF.data());
            // Writing this data to a float array
            for(auto n = 0; n < hrirSize; ++n)
            {
                buffer.getWritePointer(c)[n] = HRTF[n];
            }
        }
    }
}

