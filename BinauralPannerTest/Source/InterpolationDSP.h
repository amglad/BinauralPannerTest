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
#include <complex>
#include <vector>
#include <array>
#include <JuceHeader.h>

class InterpolationDSP
{
public:
    // Designates size of the fft window and the number of points on which it will operate. Corresponds to 2 to the power of order
    static constexpr auto fftOrder = 11;
    // Left bit shift operator which produces 2048 as binary number 100,000,000,000
    static constexpr auto fftSize = 1 << fftOrder;
    

public:
    InterpolationDSP(); // Constructor
    
    // Interpolates and covolves in 1
    void getHRIR(float az, float el, float d, juce::AudioBuffer<float> & buffer);
    
    int hrirSize = 2048;
    
    
private:
    float az;
    float el;
    float d;
    std::string name;
    std::string nameLow;
    std::string nameHigh;
    
    BasicSOFA::BasicSOFA sofa;
    
    // Creating HRTF bucket to fill in with frequency data
    std::array<float, fftSize * 2> HRTFLow;
    std::array<float, fftSize * 2> HRTFHigh;
    std::array<float, fftSize * 2> HRTF;

    // fft object
    juce::dsp::FFT fft;
    
};
