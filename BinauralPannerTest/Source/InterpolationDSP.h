/*
  ==============================================================================

    InterpolationDSP.h
    Created: 9 Apr 2023 6:21:53pm
    Author:  Mitch Glad

  ==============================================================================
*/

#pragma once
#include "BasicSOFA.hpp"
#include "BasicSOFAPriv.hpp"
#include "AudioFFT.h"
#include "math.h"
#include <iostream>

class InterpolationDSP
{
    
public:
    InterpolationDSP(); // Constructor
    
    // Returns the frequency domain HRTF that is interpolated
    std::vector<std::vector<float>> interpolate(int az, int el, float d, int buffer);
    // Returns the frequency domain HRTF
    std::vector<std::vector<float>> getHRIRs(int az, int el, float d, int buffer);
    // Convolves the signal with the HRTF provided
    std::vector<std::vector<float>> convolve(int az, int el, float d, int buffer, std::vector<std::vector<float>> signal, std::vector<std::vector<float>> HRTF);
    

    
    
    
    
private:
    float Fs;
    float M;
    int bufferSize;
    
    int az;
    int el;
    float d;
    
    float dMod;
    
    BasicSOFA::BasicSOFA sofa;
    audiofft::AudioFFT fft;
    
    
};
