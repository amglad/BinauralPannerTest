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
//    bool success = sofa.readSOFAFile("/Users/mitchglad/BinauralPannerTest/BinauralPannerTest/Source/SOFA/SmallTheaterHRIRsPart2_1.0.sofa");
//    bool success2;
//    if(!success)
//        success2 = sofa.readSOFAFile("/Users/erictarr/BinauralPannerTest/BinauralPannerTest/Source/SOFA/SmallTheaterHRIRsPart2_1.0.sofa");
//    if(!success2)
//        return;

}


void InterpolationDSP::getHRIR(float az, float el, float d, juce::AudioBuffer<float> & buffer)
{
   // int dI = static_cast <int> (d);
    if (az < 0) {
        az = az + 360;
    }
    if (el < 0) {
        el = el + 360;
    }
    
    if(d == 2.f || d == 6.f || d == 10.f || d == 14.f) // If in HRIR database
    {
        for (int c = 0; c < buffer.getNumChannels() ; c++){
            // Getting HRIRs from .sofa file
            //            const double *hrir = sofa.getHRIR(c, az, el, d);
            
            int azInt = static_cast <int> (az);
            int elInt = static_cast <int> (el);
            int dInt = static_cast <int> (d);
            
            std::string azSt = std::to_string(azInt);
            std::string elSt = std::to_string(elInt);
            std::string dSt = std::to_string(dInt);
            
            // Writes string
            if (c == 0)
            {
                name = "az" + azSt + "_el" + elSt + "_d" + dSt + "_L_wav";
            }
            else
            {
                name = "az" + azSt + "_el" + elSt + "_d" + dSt + "_R_wav";
            }
        
            // Getting to the right format
            const char * name1 = name.c_str();
            
//            const auto name = "_96k_Test_wav"; //BinaryData::getNamedResourceOriginalFilename(BinaryData::namedResourceList[0]);
            // Setting size and reading binary data
            int irDataSize = 4140;
            auto* irData = BinaryData::getNamedResource(name1,irDataSize);
            

            // Writing this data to a float array
            for(int n = 0; n < hrirSize; ++n)
            {
//                buffer.getWritePointer(c)[n] = static_cast<float> (hrir[n]);
                buffer.getWritePointer(c)[n] = static_cast<float> (irData[n]);
            }
        }
    }
    else // If we need to interpolate
    {
        // Finding the mod of our distance
        float dMod = fmod((d-2.f),4.f);
        
        // Getting the surrounding distances
        int dLow = d - dMod;
        int dHigh = d + (4 - dMod);
        
        // Finding weights
        float dLowW = abs((4.f - dMod)/4.f);
        float dHighW = abs(dMod/4.f);
            
        for (int c = 0; c < buffer.getNumChannels() ; c++){
            // Getting HRIRs from .sofa file
//            auto *hrirLow = sofa.getHRIR(c, az, el, dLow);
//            auto *hrirHigh = sofa.getHRIR(c, az, el, dHigh);
            
        
            
            // Turning parameters into strings
            std::string azSt = std::to_string(static_cast <int> (az));
            std::string elSt = std::to_string(static_cast <int> (el));
            std::string dLowSt = std::to_string(static_cast <int> (dLow));
            std::string dHighSt = std::to_string(static_cast <int> (dHigh));
            
            // Writes string
            if (c == 0)
            {
                nameLow = "az" + azSt + "_el" + elSt + "_d" + dLowSt + "_L_wav";
                nameHigh = "az" + azSt + "_el" + elSt + "_d" + dHighSt + "_L_wav";
            }
            else
            {
                nameLow = "az" + azSt + "_el" + elSt + "_d" + dLowSt + "_R_wav";
                nameHigh = "az" + azSt + "_el" + elSt + "_d" + dHighSt + "_L_wav";
            }
            
            // Getting to the right format
            const char * nameLow1 = nameLow.c_str();
            const char * nameHigh1 = nameHigh.c_str();
            
//            const auto name = "_96k_Test_wav"; //BinaryData::getNamedResourceOriginalFilename(BinaryData::namedResourceList[0]);
            // Setting size and reading binary data
            int irDataSize = 4140;
            auto* irDataLow = BinaryData::getNamedResource(nameLow1,irDataSize);
            auto* irDataHigh = BinaryData::getNamedResource(nameHigh1,irDataSize);
            
            
            
            
            
            // Writing this data to the bucket HRTFLow
            for(int n = 0; n < hrirSize; ++n)
            {
                HRTFLow[n] = irDataLow[n];
            }
            
            // Writing this data to the bucket HRTFHigh
            for(int n = 0; n < hrirSize; ++n)
            {
                HRTFHigh[n] = irDataHigh[n];
            }

            // FFT Calculations
            fft.performRealOnlyForwardTransform(HRTFLow.data(),true);
            fft.performRealOnlyForwardTransform(HRTFHigh.data(),true);
            
            // Weighting
            for(int n = 0; n < hrirSize * 2; ++n)
            {
                HRTF[n] = (HRTFLow[n] * dLowW) + (HRTFHigh[n] * dHighW);
            }
            
            // IFFT
            fft.performRealOnlyInverseTransform(HRTF.data());
            
            // Writing this data to a float array
            for(int n = 0; n < hrirSize; ++n)
            {
                buffer.getWritePointer(c)[n] = HRTF[n];
            }
        }
    }
}


//void InterpolationDSP::crossfade(juce::AudioBuffer<float> & contextStoreBuffer,
//                                 juce::AudioBuffer<float> & contextBuffer,
//                                 juce::AudioBuffer<float> & buffer)
//{
//    int numSamples = contextBuffer.getNumSamples();
//
//    for (int channel = 0; channel < 2; ++channel) // loop through both channels
//    {
//        for (int n = 0; n < numSamples; ++n)
//        {
//            buffer.getWritePointer(channel)[n] =
//                                  (1 - n / numSamples) * contextStoreBuffer.getSample(channel, n)
//                                  + (n / numSamples) * contextBuffer.getSample(channel, n);
//        }
//    }
//}
