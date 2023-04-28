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
    bool success = sofa.readSOFAFile("/Users/mitchglad/BinauralPannerTest/BinauralPannerTest/Source/SOFA/ind01_h_sph_equivert_dir614_dist91.sofa");
    bool success2;
    if(!success)
        success2 = sofa.readSOFAFile("/Users/erictarr/BinauralPannerTest/BinauralPannerTest/Source/SOFA/ind01_h_sph_equivert_dir614_dist91.sofa");
    if(!success2)
        return;

}


void InterpolationDSP::getHRIR(float az, float el, float d, juce::AudioBuffer<float> & buffer)
{
    if(d == 0.1f || d == 0.2f || d == 0.3f || d ==0.4 || d == 0.5f || d == 0.6f || d == 0.7f || d ==0.8 || d == 0.9f || d == 1.0f) // If in HRIR database
    {
        for (int c = 0; c < buffer.getNumChannels() ; c++){
            // Getting HRIRs from .sofa file
            const double *hrir = sofa.getHRIR(c, az, el, d);
            
            // Writing this data to a float array
            for(int n = 0; n < hrirSize; ++n)
            {
                buffer.getWritePointer(c)[n] = static_cast<float> (hrir[n]);
            }
        }
    }
    else // If we need to interpolate
    {
        // Finding the mod of our distance
        float dMod = fmod((d-0.1),0.1f);
        
        // Getting the surrounding distances
        int dLow = d - dMod;
        int dHigh = d + (0.1 - dMod);
        
        // Finding weights
        float dLowW = abs((0.1f - dMod)/0.1f);
        float dHighW = abs(dMod/0.1f);
            
        for (int c = 0; c < buffer.getNumChannels() ; c++){
            // Getting HRIRs from .sofa file
            const double *hrirLow = sofa.getHRIR(c, az, el, dLow);
            const double *hrirHigh = sofa.getHRIR(c, az, el, dHigh);
            
            // Writing this data to the bucket HRTFLow
            for(int n = 0; n < fftSize; ++n)
            {
                HRTFLow[n] = static_cast<float> (hrirLow[n]);
            }
            
            // Writing this data to the bucket HRTFHigh
            for(int n = 0; n < fftSize; ++n)
            {
                HRTFHigh[n] = static_cast<float> (hrirHigh[n]);
            }
            
            // FFT Calculations
            fft.performRealOnlyForwardTransform(HRTFLow.data(),true);
            fft.performRealOnlyForwardTransform(HRTFHigh.data(),true);
            
            // Weighting
            for(int n = 0; n < fftSize * 2; ++n)
            {
                HRTF[n] = (HRTFLow[n] * dLowW) + (HRTFHigh[n] * dHighW);
            }
            
            // IFFT
            fft.performRealOnlyInverseTransform(HRTF.data());
            
            // Writing this data to a float array
            for(int n = 0; n < hrirSize; ++n)
            {
                buffer.getWritePointer(c)[n] = HRTF[n];
            }
        }
    }
}


//void InterpolationDSP::crossfade(juce::AudioBuffer<float> & contextStoreBuffer,
//                                 juce::AudioBuffer<float> & contextBuffer,
//                                 juce::AudioBuffer<float> & buffer)
//{
//    int numSamples = contextBuffer.getNumSamples();
//
//    for (int channel = 0; channel < 2; ++channel) // loop through both channels
//    {
//        for (int n = 0; n < numSamples; ++n)
//        {
//            buffer.getWritePointer(channel)[n] =
//                                  (1 - n / numSamples) * contextStoreBuffer.getSample(channel, n)
//                                  + (n / numSamples) * contextBuffer.getSample(channel, n);
//        }
//    }
//}

