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

std::vector<std::vector<float>> InterpolationDSP::interConv(int az, int el, float d, int buffer, std::vector<float> signal)
{
    // Creating empty real and imaginary vectors for fft interpolation
    std::vector<float> reLow(audiofft::AudioFFT::ComplexSize(buffer));
    std::vector<float> imLow(audiofft::AudioFFT::ComplexSize(buffer));
    std::vector<float> reHigh(audiofft::AudioFFT::ComplexSize(buffer));
    std::vector<float> imHigh(audiofft::AudioFFT::ComplexSize(buffer));
    
    // Creating empty real and imaginary vectors for fft no interpolation
    std::vector<float> reH(audiofft::AudioFFT::ComplexSize(buffer));
    std::vector<float> imH(audiofft::AudioFFT::ComplexSize(buffer));
    
    // Creating empty real and imaginary vectors for fft signal
    std::vector<float> re(audiofft::AudioFFT::ComplexSize(buffer));
    std::vector<float> im(audiofft::AudioFFT::ComplexSize(buffer));
    
    // Creating empty real and imaginary vectors for ifft output signal
    std::vector<float> reOut(audiofft::AudioFFT::ComplexSize(buffer));
    std::vector<float> imOut(audiofft::AudioFFT::ComplexSize(buffer));
    
    // Allocating array for the final array
    std::vector<std::vector<float>> output(buffer);
    
    
    
    if(d == 2 || d == 6 || d == 10 || d == 14) // If in HRIR database
    {
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
                fft.fft(&signal[i],&re[i],&im[i]);
            }
            
            // Frequency domain convolution
            for(auto i = 0; i < buffer; i++)
            {
                output[channel][i] = signal[i] * hrirF[i];
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
                fft.fft(&signal[i],&re[i],&im[i]);
            }
            
            // Calculating interpolation and convolving
            for(auto i = 0; i < buffer; i++)
            {
                output[channel][i] = signal[i] * (hLowF[i] * dLowW + hHighF[i] * dHighW);
            }
            
            
            // Inverse Fourier Transform (Do we need a different re and im this time? I recreated them as zeros but idk if that will work)
            for(auto i = 0; i < buffer; i++)
            {
                fft.ifft(&output[channel][i],&reOut[i],&imOut[i]);
            }
        }
    }
    
    // Return the value
    return output;
}










// Functions that are all individual (helped me picture what I needed from each to combine into the function above)
/*----------------------------------------------------------------------------------------------------------------*/
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
            HRTF[channel][i] = hrirF[i];
        }
    }
    
    // Return the value
    return HRTF;
}
    




std::vector<std::vector<float>> InterpolationDSP::convolve(int buffer, std::vector<float> signal, std::vector<std::vector<float>> HRTF)
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
            fft.fft(&signal[i],&re[i],&im[i]);
        }
        
        // Frequency domain multiplication for convolution
        for(auto i = 0; i < buffer; i++)
        {
            output[channel][i] = signal[i] * HRTF[channel][i];
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



