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
    InterpolationDSP(); // Constructor
    
    // Interpolates and covolves in 1
    std::vector<std::vector<float>> interConv(int az, int el, float d, int buffer, std::vector<float> signal);
    
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    
    void pushNextSampleIntoFifo (float sample) noexcept;
    
    
    
    // Functions that are all individual (helped me picture what I needed from each to combine into the function above)
    // Returns the frequency domain HRTF that is interpolated
    std::vector<std::vector<float>> interpolate(int az, int el, float d, int buffer);
    // Returns the frequency domain HRTF
    std::vector<std::vector<float>> getHRIRs(int az, int el, float d, int buffer);
    // Convolves the signal with the HRTF provided
    std::vector<std::vector<float>> convolve(int buffer, std::vector<float> signal, std::vector<std::vector<float>> HRTF);
    
    
    
    
    
public:
    // Designates size of the fft window and the number of points on which it will operate. Corresponds to 2 to the power of order
    static constexpr auto fftOrder = 10;
    // Left bit shift operator which produces 1024 as binary number 10000000000
    static constexpr auto fftSize = 1<< fftOrder;
    

    
    
    
    
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
    
    // fft object
    juce::dsp::FFT fftLeft;
    juce::dsp::FFT fftRight;
    juce::dsp::FFT fftSignal;
    // 1024 size which will contain incoming audio data in samples
    std::array<float, fftSize> fifo;
    // 2048 size contains results of fft calculations
    std::array<float,fftSize> fftData;
    // Temporary index
    int fifoIndex = 0;
    // Tells us whether the next FFt block is ready
    bool nextFFTBlockReady = false;
    
};
