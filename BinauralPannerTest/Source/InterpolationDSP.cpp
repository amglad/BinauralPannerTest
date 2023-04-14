/*
  ==============================================================================

    InterpolationDSP.cpp
    Created: 9 Apr 2023 6:21:53pm
    Author:  Mitch Glad

  ==============================================================================
*/

#include "InterpolationDSP.h"
#include "AudioFFT.h"

float InterpolationDSP::processSample(const float* x, float* re, float* im)
{
    const size_t fftSize = 1024; // Needs to be power of 2!
    
    audiofft::AudioFFT fft;
    
    fft.fft(x, re, im);
    
    /* mess with the data in here?
     
     
     
     
     */
    
    
    fft.ifft(x, re, im);
    
}
