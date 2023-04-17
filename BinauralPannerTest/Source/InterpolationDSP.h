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
    std::array<std::array<float, 1024>, 2> interConv(int az, int el, float d, int buffer, int channel, std::array<float,1024 * 2> signal);
    
    void pushNextSampleIntoFifo (float sample, std::array<float,1024 * 2> fftData) noexcept;
    
    void fourierTransform (std::array<float,1024 * 2> fftData);
    
    void inverseFourierTransform (std::array<float,1024 * 2> fftData);
    
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    
    
    
    
    
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
//    audiofft::AudioFFT fft;
    
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
