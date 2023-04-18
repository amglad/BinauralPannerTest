/*
  ==============================================================================

    InterpolationDSP.cpp
    Created: 9 Apr 2023 6:21:53pm
    Author:  Mitch Glad

  ==============================================================================
*/

#include "InterpolationDSP.h"

InterpolationDSP::InterpolationDSP() : fft (fftOrder)
{
    
    // Reading SOFA file
    bool success = sofa.readSOFAFile("/Users/mitchglad/BinauralPannerTest/BinauralPannerTest/Source/SOFA/SmallTheaterHRIRs_1.0.sofa");
    if(!success)
        return;
    
    // Getting sampling rate and number of files
    //Fs = sofa.getFs();
    M = sofa.getM();
}

void InterpolationDSP::interConv(int az, int el, float d, int numSamples, int channel, std::array<float,fftSize * 2> signal, juce::AudioBuffer<float>& buffer) // Not sure which amount of signal (buffer or just 1024 we will be using)
{
    
    // For when working with trying to keep the HRTF the same if it's not changed with an if statement, how do we keep track of the previous HRTF? The easiest way to do it would be to have the HRTF stored in the frequency domain so that if it doesn't change, all we have to do is the multiplication of the frequency domain signal to convolve then an inverse fft. Would we be able to write a new function for that? Then we could have the if statement in the process block and run this if it did change but the other one if it didn't. This would also make it so we don't have to recalculate the interpolation for the distance. Also, thank fuck we aren't doing this with 3D interpolation.
    if(d == 2 || d == 6 || d == 10 || d == 14) // If in HRIR database
    {
        // Getting HRIRs from .sofa file
        const double *hrir = sofa.getHRIR(channel, az, el, d);
        
        // Creating HRTF bucket to fill in with frequency data
        std::array<float, fftSize * 2> HRTF;
            
        // Writing this data to the bucket HRTF
        for(auto n = 0; n < numSamples; ++n)
        {
            HRTF[n] = hrir[n];
        }
        
        // FFT Calculation
        fourierTransform(HRTF);
        
        // Convolution
        for(auto n = 0; n < fftSize * 2; ++n)
        {
            outputFreq[n] = HRTF[n] * signal[n];
        }
        
        inverseFourierTransform(outputFreq);
        
        // Writing to buffer
        for(auto n = 0; n < numSamples; ++n)
        {
            buffer.getWritePointer(channel)[n] = outputFreq[n];
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
        const double dLowW = abs((4 - dMod)/4);
        const double dHighW = abs(dMod/4);
            
        // Getting HRIRs from .sofa file
        const double *hrirLow = sofa.getHRIR(channel, az, el, dLow);
        const double *hrirHigh = sofa.getHRIR(channel, az, el, dHigh);
        
        
        // Writing this data to the bucket HRTFLow
        for(auto n = 0; n < fftSize; ++n)
        {
            HRTFLow[n] = hrirLow[n];
        }
        // FFT Calculation
        fourierTransform(HRTFLow);
        
        // Writing this data to the bucket HRTFHigh
        for(auto n = 0; n < fftSize; ++n)
        {
            HRTFHigh[n] = hrirHigh[n];
        }
        // FFT Calculation
        fourierTransform(HRTFHigh);
        
        // The for loops above and below this are an example of not knowing how many samples to run these for
        
        // Weighting
        for(auto n = 0; n < fftSize * 2; ++n)
        {
            HRTF[n] = (HRTFLow[n] * dLowW) + (HRTFHigh[n] * dHighW);
        }
        
        // Convolution
        for(auto n = 0; n < fftSize * 2; ++n)
        {
            outputFreq[n] = HRTF[n] * signal[n];
        }
        
        inverseFourierTransform(outputFreq);
        
        // Writing to buffer
        for(auto n = 0; n < numSamples; ++n)
        {
            buffer.getWritePointer(channel)[n] = outputFreq[n];
        }
        
    }
}



void InterpolationDSP::fourierTransform (std::array<float,fftSize * 2> fftData)
{
    fft.performRealOnlyForwardTransform(fftData.data());
}

void InterpolationDSP::inverseFourierTransform (std::array<float,fftSize * 2> fftData)
{
    fft.performRealOnlyInverseTransform(fftData.data());
}

                                       
void InterpolationDSP::pushNextSampleIntoFifo (float sample, std::array<float,fftSize * 2> fftData) noexcept
{
    // if the fifo contains enough data, set a flag to say
    // that the next line should now be rendered..
    // If the fifoIndex is equal to the size that we want the fft to be (buffers?)
    if (fifoIndex == fftSize)       // [8]
    {
        // The nextFFTBlockReady is set to false as a default
       if (! nextFFTBlockReady)    // [9]
       {
           // Copies the data from fifo into the fftData which is twice the size
           std::fill (fftData.begin(), fftData.end(), 0.0f);
           std::copy (fifo.begin(), fifo.end(), fftData.begin());
           nextFFTBlockReady = true; // Do we need to reset this value?
       }

       fifoIndex = 0;
    }

    // If not full yet, add the sample to the next index position
    fifo[(size_t) fifoIndex++] = sample; // [9]
}


void InterpolationDSP::reConv(int numSamples, int channel, std::array<float, fftSize * 2> signal, juce::AudioBuffer<float> &buffer)
{
    // Convolution
    for(auto n = 0; n < fftSize * 2; ++n)
    {
        outputFreq[n] = HRTF[n] * signal[n];
    }

    inverseFourierTransform(outputFreq);

    // Writing to buffer
    for(auto n = 0; n < numSamples; ++n)
    {
        buffer.getWritePointer(channel)[n] = outputFreq[n];
    }
}

