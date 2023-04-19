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
//    static constexpr auto fftSize = 1 << fftOrder;
    static constexpr auto fftSize = 2048;
    

public:
    InterpolationDSP(); // Constructor
    
    // Interpolates and covolves in 1
    void interConv(int az, int el, float d, int numSamples, int channel, std::array<float,fftSize * 2> signal, juce::AudioBuffer<float>& buffer);
    
    void reConv(int numSamples, int channel, std::array<float,fftSize * 2> signal, juce::AudioBuffer<float>& buffer);
    
    void pushNextSampleIntoFifo (float sample, std::array<float,fftSize * 2> fftData) noexcept;
    
    void fourierTransform (std::array<float,fftSize * 2> fftData);
    
    void inverseFourierTransform (std::array<float,fftSize * 2> fftData);
    

private:
    float Fs;
    float M;
    int bufferSize;
    
    int az;
    int el;
    float d;
    
    float dMod;
    
    BasicSOFA::BasicSOFA sofa;
//    audiofft::AudioFFT fft;
    
    // Creating final output (How do we make it variable with the buffer size?)
    std::array<float, fftSize * 2> outputFreq;
    
    // Creating HRTF bucket to fill in with frequency data
    std::array<float, fftSize * 2> HRTFLow;
    std::array<float, fftSize * 2> HRTFHigh;
    std::array<float, fftSize * 2> HRTF;
    
    
    
    
    
    // fft object
    juce::dsp::FFT fft;
    // 1024 size which will contain incoming audio data in samples
    std::array<float, fftSize> fifo;
    // 2048 size contains results of fft calculations
    std::array<float,fftSize * 2> fftData;
    // Temporary index
    int fifoIndex = 0;
    // Tells us whether the next FFt block is ready
    bool nextFFTBlockReady = false;
    
};
