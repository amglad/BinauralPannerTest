/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class BinauralPannerTestAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                                public juce::ValueTree::Listener
                                               // public juce::Slider::Listener
{
public:
    BinauralPannerTestAudioProcessorEditor (BinauralPannerTestAudioProcessor&);
    ~BinauralPannerTestAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    // virtual void sliderValueChanged(juce::Slider* slider) override;
    
    int windowWidth = 600;
    int windowHeight = 300;
    
    void valueTreePropertyChanged (juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property) override;
    void valueTreeRedirected (juce::ValueTree &treeWhichHasBeenChanged) override;
    
    // slider listener for just the gain knob
//    void sliderValueChanged(juce::Slider * slider) override;
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    BinauralPannerTestAudioProcessor& audioProcessor;
    
    juce::Slider azKnob;
    juce::Slider elKnob;
    juce::Slider dSlider;
  //  juce::Slider gainKnob;
    
    juce::Label azKnobLabel;
    juce::Label elKnobLabel;
    juce::Label dSliderLabel;
 //   juce::Label gainLabel;
    
    juce::Label title;
    
    juce::LookAndFeel_V4 lookAndFeelV4;
    
    juce::ValueTree tree;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BinauralPannerTestAudioProcessorEditor)
    
public:
    
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sliderAttachment;

};
