/*
  ==============================================================================

    InterpolationDSP.cpp
    Created: 9 Apr 2023 6:21:53pm
    Author:  Mitch Glad

  ==============================================================================
*/

#include "InterpolationDSP.h"

InterpolationDSP::InterpolationDSP()
{
    
    // Reading SOFA file
    bool success = sofa.readSOFAFile("/Users/erictarr/BinauralPannerTest/BinauralPannerTest/Source/SOFA/SmallTheaterHRIRs_1.0.sofa");
    if(!success)
        return;
    
    // Getting sampling rate and number of files
    //Fs = sofa.getFs();
    M = sofa.getM();
}

std::vector<std::vector<float>> InterpolationDSP::interpolate(int az, int el, float d, int buffer)
{
    // Finding the mod of our distance
    float dMod = fmod((d-2),4);
    
    // Getting the surrounding distances
    int dLow = d - dMod;
    int dHigh = d + (4 - dMod);
    
    // Finding weights
    const double dLowW = abs(dMod/4);
    const double dHighW = abs(dMod/4);
    
    // Creating empty real and imaginary vectors for fft
    std::vector<float> reLow(audiofft::AudioFFT::ComplexSize(buffer));
    std::vector<float> imLow(audiofft::AudioFFT::ComplexSize(buffer));
    std::vector<float> reHigh(audiofft::AudioFFT::ComplexSize(buffer));
    std::vector<float> imHigh(audiofft::AudioFFT::ComplexSize(buffer));
    
    // Allocating array for the interpolation
    std::vector<std::vector<float>> Hinterp(buffer);
    
    // Looping for both channels
    for(auto channel = 0; channel < 2; channel++)
    {
        // Getting HRIRs from .sofa file
        const double *hLow = sofa.getHRIR(channel, az, el, dLow);
        const double *hHigh = sofa.getHRIR(channel, az, el, dHigh);
        
        // Casting to a float
        const float *hLowF = (const float *) hLow;
        const float *hHighF = (const float *) hHigh;
        
        // Initializing fft size
        fft.init(buffer);
        
        // Performing fft
        for(auto i = 0; i < buffer; i++)
        {
            fft.fft(&hLowF[i],&reLow[i],&imLow[i]);
            fft.fft(&hHighF[i],&reHigh[i],&imHigh[i]);
        }
        
        // Calculating interpolation
        for(auto i = 0; i < buffer; i++)
        {
            Hinterp[channel][i] = hLowF[i] * dLowW + hHighF[i] * dHighW;
        }
    }
    
    // Return the value
    return Hinterp;
}




std::vector<std::vector<float>> InterpolationDSP::getHRIRs(int az, int el, float d, int buffer)
{
    
    // Creating empty real and imaginary vectors for fft
    std::vector<float> re(audiofft::AudioFFT::ComplexSize(buffer));
    std::vector<float> im(audiofft::AudioFFT::ComplexSize(buffer));
    
    // Allocating array for the final array
    std::vector<std::vector<float>> HRTF(buffer);
    
    // Looping for both channels
    for(auto channel = 0; channel < 2; channel++)
    {
        // Getting HRIRs from .sofa file
        const double *hrir = sofa.getHRIR(channel, az, el, d);
        // Casting to a float
        const float *hrirF = (const float *) hrir;
        
        // Initializing fft size
        fft.init(buffer);
        
        // Performing fft
        for(auto i = 0; i < buffer; i++)
        {
            fft.fft(&hrirF[i],&re[i],&im[i]);
        }
        
        // Placing them into 2D array
        for(auto i = 0; i < buffer; i++)
        {
            HRTF[channel][i] = hrir[i];
        }
    }
    
    // Return the value
    return HRTF;
}
    




std::vector<std::vector<float>> InterpolationDSP::convolve(int az, int el, float d, int buffer, std::vector<std::vector<float>> signal, std::vector<std::vector<float>> HRTF)
{
    
    // Creating empty real and imaginary vectors for fft
    std::vector<float> re(audiofft::AudioFFT::ComplexSize(buffer));
    std::vector<float> im(audiofft::AudioFFT::ComplexSize(buffer));
    
    // Allocating array for the final array
    std::vector<std::vector<float>> output(buffer);
    
    // Looping for both channels
    for(auto channel = 0; channel < 2; channel++)
    {
        
        // Initializing fft size
        fft.init(buffer);
        
        // Performing fft
        for(auto i = 0; i < buffer; i++)
        {
            fft.fft(&signal[channel][i],&re[i],&im[i]);
        }
        
        // Frequency domain multiplication for convolution
        for(auto i = 0; i < buffer; i++)
        {
            output[channel][i] = signal[channel][i] * HRTF[channel][i];
        }
        
        // Inverse Fourier Transform (Do we need a different re and im this time?)
        for(auto i = 0; i < buffer; i++)
        {
            fft.ifft(&output[channel][i],&re[i],&im[i]);
        }
        
    }
    
    // Return the value
    return output;
}

