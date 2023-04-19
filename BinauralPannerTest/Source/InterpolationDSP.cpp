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


juce::AudioBuffer<float> InterpolationDSP::getHRIR(int az, int el, float d, int channel)
{
    if(d == 2 || d == 6 || d == 10 || d == 14) // If in HRIR database
    {
        // Getting HRIRs from .sofa file
        const double *hrir = sofa.getHRIR(channel, az, el, d);
        
        // Writing this data to a float array
//        for(auto n = 0; n < hrirSize; ++n)
//        {
//            hrirF[n] = hrir[n];
//        }

        IRbuffer.setDataToReferTo(hrirF, 2, hrirF[0], 2048);
        
        // Returning Data
        return IRbuffer;
        
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
            
        // Getting HRIRs from .sofa file
        const double *hrirLow = sofa.getHRIR(channel, az, el, dLow);
        const double *hrirHigh = sofa.getHRIR(channel, az, el, dHigh);
        
        
        // Writing this data to the bucket HRTFLow
        for(auto n = 0; n < fftSize; ++n)
        {
            HRTFLow[n] = hrirLow[n];
        }
        // FFT Calculation
        fft.performRealOnlyForwardTransform(HRTFLow.data());
        
        // Writing this data to the bucket HRTFHigh
        for(auto n = 0; n < fftSize; ++n)
        {
            HRTFHigh[n] = hrirHigh[n];
        }
        // FFT Calculation
        fft.performRealOnlyForwardTransform(HRTFHigh.data());
        
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
            IRbuffer.getWritePointer(channel)[n] = HRTF[n];
        }
        
        // Returning Data
        return IRbuffer;
        
    }
}

