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
    
    const double getLow(float d); // Grabs the
    
    const double getHigh(float d);
    
    const double interpolate2(double dLow, double dHigh, double dLowW, double dHighW);
    
    const double interpolate(int az, int el, float d);
    
    const double getWeightHigh(float d, float dMod);
    
    const double getWeightLow(float d, float dMod);

    
    
    
    
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
