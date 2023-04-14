/*
  ==============================================================================

    InterpolationDSP.h
    Created: 9 Apr 2023 6:21:53pm
    Author:  Mitch Glad

  ==============================================================================
*/

#pragma once

class InterpolationDSP
{
    
public:
    
    float processSample(const float* a, float* b, float* c);
    
    
    
private:
    
    int bufferSize = 1024;
    
    
};
