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
    
    float azStore = 2.f;
    float elStore = 2.f;
    float dStore = 4.f;
    juce::AudioBuffer<float> hrir;
    juce::dsp::Convolution conv;

private:
    
    InterpolationDSP interp;
    
    
 
public:
    
    juce::AudioProcessorValueTreeState state;
    
    // function to fill the value tree
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    //============================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BinauralPannerTestAudioProcessor)
};
