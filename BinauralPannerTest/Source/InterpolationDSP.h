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
    
    int hrirSize = 2048;
    

public:
    InterpolationDSP(); // Constructor
    
    // Interpolates and covolves in 1
    void getHRIR(int az, int el, float d, juce::AudioBuffer<float> & buffer);
    

private:
    int az;
    int el;
    float d;
    
    float dMod;
    
    BasicSOFA::BasicSOFA sofa;
    
    // Creating HRTF bucket to fill in with frequency data
    std::array<float, fftSize * 2> HRTFLow;
    std::array<float, fftSize * 2> HRTFHigh;
    std::array<float, fftSize * 2> HRTF;
    std::array<float, fftSize * 2> hrir;
    std::array<float, fftSize> hrirF;

    // fft object
    juce::dsp::FFT fft;
    
    // audio buffer to store impulse response in
    juce::AudioBuffer<float> IRbuffer;
    
};
