/*
  ==============================================================================

    InterpolationDSP.cpp
    Created: 9 Apr 2023 6:21:53pm
    Author:  Mitch Glad

  ==============================================================================
*/

#include "InterpolationDSP.h"

InterpolationDSP::InterpolationDSP()
{
    
    // Reading SOFA file
    bool success = sofa.readSOFAFile("/Users/erictarr/BinauralPannerTest/BinauralPannerTest/Source/SOFA/SmallTheaterHRIRs_1.0.sofa");
    if(!success)
        return;
    
    // Getting sampling rate and number of files
    Fs = sofa.getFs();
    M = sofa.getM();
}

const double InterpolationDSP::interpolate(int az, int el, float d)
{
    // Finding the mod of our distance
    float dMod = fmod((d-2),4);
    
    // Getting the surrounding distances
    int dLow = d - dMod;
    int dHigh = d + (4 - dMod);
    
    // Finding weights
    const double dLowW = abs(dMod/4);
    const double dHighW = abs(dMod/4);
    
    auto channel = 0;
    
    // Getting HRIRs from .sofa file
    const double *hLow = sofa.getHRIR(channel, az, el, dLow);
    const double *hHigh = sofa.getHRIR(channel, az, el, dHigh);
    
//    for(auto i = 0; i < sofa.getN(); i++)
//        auto x = hLow[i];
    
    // FFT Work
    const size_t fftSize = 2048;
    
//    std::vector<float> input(fftSize, 0.0f);
    std::vector<float> re(audiofft::AudioFFT::ComplexSize(fftSize));
    std::vector<float> im(audiofft::AudioFFT::ComplexSize(fftSize));
    std::vector<float> output(fftSize);
    
    fft.init(2048);
    
//    const double *HLow = fft.fft(input.data(), re.data(), im.data());
//    const double *HHigh = fft.ifft(output.data(), re.data(), im.data());
    
    
//    // Getting weights of the HRIRs in the frequency domain
//    const double *HRIR = HLow * dLowW + HHigh * dHighW;
//
//    return HRIR;
}

