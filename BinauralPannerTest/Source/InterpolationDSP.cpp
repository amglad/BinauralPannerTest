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
    bool success = sofa.readSOFAFile("/Users/erictarr/BinauralPannerTest/BinauralPannerTest/Source/SOFA/SmallTheaterHRIRs_1.0.sofa");
    if(!success)
        return;
    
    // Getting sampling rate and number of files
    //Fs = sofa.getFs();
    M = sofa.getM();
}

std::array<std::array<float, 1024>, 2> InterpolationDSP::interConv(int az, int el, float d, int buffer, int channel, std::array<float,1024 * 2> signal) // Not sure which amount of signal (buffer or just 1024 we will be using)
{
    // Creating final output
    std::array<std::array<float, 1024>, 2> output;
    
    if(d == 2 || d == 6 || d == 10 || d == 14) // If in HRIR database
    {
        // Getting HRIRs from .sofa file
        const double *hrir = sofa.getHRIR(channel, az, el, d);
        
        // Creating HRTF bucket to fill in with frequency data
        std::array<float, 1024 * 2> HRTF;
            
        // Writing this data to the bucket HRTF
        for(auto i = 0; i < buffer; ++i)
        {
            pushNextSampleIntoFifo(hrir[i], HRTF);
        }
        
        // FFT Calculation
        fourierTransform(HRTF);
        
        // Convolution
        for(auto i = 0; i < 1024; ++i)
        {
            output[i][channel] = HRTF[i] * signal[i];
        }
        
        // Returning output
        return output;
        
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
        
        // Creating HRTF bucket to fill in with frequency data
        std::array<float, 1024 * 2> HRTFLow;
        std::array<float, 1024 * 2> HRTFHigh;
        std::array<float, 1024 * 2> HRTF;
        
        
        // Writing this data to the bucket HRTFLow
        for(auto i = 0; i < buffer; ++i)
        {
            pushNextSampleIntoFifo(hrirLow[i], HRTFLow);
        }
        // FFT Calculation
        fourierTransform(HRTFLow);
        
        // Writing this data to the bucket HRTFHigh
        for(auto i = 0; i < buffer; ++i)
        {
            pushNextSampleIntoFifo(hrirHigh[i], HRTFHigh);
        }
        // FFT Calculation
        fourierTransform(HRTFHigh);
        
        // The for loops above and below this are an example of not knowing how many samples to run these for
        
        // Weighting
        for(auto i = 0; i < 1024; ++i)
        {
            HRTF[i] = HRTFLow[i] * dLowW + HRTFHigh[i] * dHighW;
        }
        
        // Convolution
        for(auto i = 0; i < 1024; ++i)
        {
            output[i][channel] = HRTF[i] * signal[i];
        }
        
        // Not quite sure how to do inverse fft
            
        
    }
    return output;
}



void InterpolationDSP::fourierTransform (std::array<float,1024 * 2> fftData)
{
    fft.performFrequencyOnlyForwardTransform(fftData.data());
}

                                       
void InterpolationDSP::pushNextSampleIntoFifo (float sample, std::array<float,1024 * 2> fftData) noexcept
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





//void InterpolationDSP::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
//{
//   if(bufferToFill.buffer->getNumChannels() > 0)
//   {
//       auto* channelData = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);
//       
//       for (auto i = 0; i < bufferToFill.numSamples; ++i)
//       {
////            pushNextSampleIntoFifo (channelData[i]);
//       }
//   }
//}
