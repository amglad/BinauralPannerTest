/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "InterpolationDSP.h"


//==============================================================================
/**
*/
class BinauralPannerTestAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    BinauralPannerTestAudioProcessor();
    ~BinauralPannerTestAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    float azimuthAngle;
    void setAzimuth(float azimuthValue);
    
    float elevationAngle;
    void setElevation(float elevationValue);
    
    float distance;
    void setDistance(float distanceValue);
    
//    double gain_dB = 0.0;
//    void setGain(double makeupGain_Lin) {makeupGain = makeupGain_Lin;}
    
    void updateIR();
    
    float azStore = 0.f;
    float elStore = 0.f;
    float dStore = 2.f;

    double hrirFs = 96000;

    juce::AudioBuffer<float> irBuffer {2, 2048};
    
    juce::dsp::Convolution conv;

    juce::dsp::ProcessSpec spec;

    juce::AudioProcessorValueTreeState state;
    
private:
    
    InterpolationDSP interp;
    
    float makeupGain;
    
    //============================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BinauralPannerTestAudioProcessor)
    
public:
    
    // function to fill the value tree
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
};
