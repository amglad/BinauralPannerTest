/*
  ==============================================================================

    InterpolationDSP.cpp
    Created: 9 Apr 2023 6:21:53pm
    Author:  Mitch Glad

  ==============================================================================
*/

#include "InterpolationDSP.h"

InterpolationDSP::InterpolationDSP() : fftEngine (fftOrder)
{
    
    // Reading SOFA file
    bool success = sofa.readSOFAFile("/Users/erictarr/BinauralPannerTest/BinauralPannerTest/Source/SOFA/SmallTheaterHRIRs_1.0.sofa");
    if(!success)
        return;
    
    // Getting sampling rate and number of files
    //Fs = sofa.getFs();
    M = sofa.getM();
}

std::vector<std::vector<float>> InterpolationDSP::interConv(int az, int el, float d, int buffer, std::vector<float> signal)
{
    if(d == 2 || d == 6 || d == 10 || d == 14) // If in HRIR database
    {
        // Looping for both channels
        for(auto channel = 0; channel < 2; channel++)
        {
            // Getting HRIRs from .sofa file
            const double *hrir = sofa.getHRIR(channel, az, el, d);
            
            // Creating vector to fillin
            std::vector<float> hrirVec(2048);
            
            // Writing current audio block
            getNextAudioBlock(const juce::AudioSourceChannelInfo&); // We grab the new stuff from the current buffer
            
            // Writing values to float?
            for(auto i = 0; i < 2048; ++i)
            {
                hrirVec[i] = hrir[i];
            }
            
            // Writing this to the new type of variable? Do we need to create multiple instances if FIFO?
            for(auto i = 0; i < 2048; ++i)
            {
                pushNextSampleIntoFifo(hrirVec[i]);
            }
            
            // FFT Calculation?
            fftLeft.performFrequencyOnlyForwardTransform(hrirVec.data());
            
            for(auto i = 0; i < 2048; ++i)
            {
                output  = hrirVec.data() * signalVec.data();
            }
            
            
            
            
            
        }
    }
    else // If we need to interpolate
    {
        // Finding the mod of our distance
        float dMod = fmod((d-2),4);
        
        // Getting the surrounding distances
        int dLow = d - dMod;
        int dHigh = d + (4 - dMod);
        
        // Finding weights
        const double dLowW = abs(dMod/4);
        const double dHighW = abs(dMod/4);
        
        // Looping for both channels
        for(auto channel = 0; channel < 2; channel++)
        {
            // Getting HRIRs from .sofa file
            const double *hLow = sofa.getHRIR(channel, az, el, dLow);
            const double *hHigh = sofa.getHRIR(channel, az, el, dHigh);
            
            // Putting into a vector
            std::vector<float> hrirVec(
            
            
            // Casting to a float
            const float *hLowF = (const float *) hLow;
            const float *hHighF = (const float *) hHigh;
            
            // Initializing fft size
            fft.init(buffer);
        
        }
    }
    
    // Return the value
//    return output;
    
}



void InterpolationDSP::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if(bufferToFill.buffer->getNumChannels() > 0)
    {
        auto* channelData = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);
        
        for (auto i = 0; i < bufferToFill.numSamples; ++i)
        {
            pushNextSampleIntoFifo (channelData[i]);
        }
    }
}

void InterpolationDSP::pushNextSampleIntoFifo (float sample) noexcept
{
    // if the fifo contains enough data, set a flag to say
    // that the next line should now be rendered..
    if (fifoIndex == fftSize)       // [8]
    {
       if (! nextFFTBlockReady)    // [9]
       {
           std::fill (fftData.begin(), fftData.end(), 0.0f);
           std::copy (fifo.begin(), fifo.end(), fftData.begin());
           nextFFTBlockReady = true;
       }

       fifoIndex = 0;
    }

    fifo[(size_t) fifoIndex++] = sample; // [9]
}




