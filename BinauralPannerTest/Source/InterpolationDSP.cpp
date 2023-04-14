/*
  ==============================================================================

    InterpolationDSP.cpp
    Created: 9 Apr 2023 6:21:53pm
    Author:  Mitch Glad

  ==============================================================================
*/

#include <vector>

#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "InterpolationDSP.h"
#include "AudioFFT.h"

float InterpolationDSP::processSample(const float* a, float* b, float* c)
{

    audiofft::AudioFFT fft;
    
    fft.init(bufferSize);
    
    const size_t fftSize = audiofft::AudioFFT::ComplexSize(bufferSize); // Needs to be power of 2!
    
    std::vector<float> input(fftSize, 0.0f);
    std::vector<float> re(audiofft::AudioFFT::ComplexSize(fftSize));
    std::vector<float> im(audiofft::AudioFFT::ComplexSize(fftSize));
    std::vector<float> output(fftSize);
    

    fft.fft(input.data(), re.data(), im.data());
    fft.ifft(output.data(), re.data(), im.data());
     
     
     

    
    
    // fft.ifft(x, re, im);
    
}
