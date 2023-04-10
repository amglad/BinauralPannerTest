/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BinauralPannerTestAudioProcessorEditor::BinauralPannerTestAudioProcessorEditor (BinauralPannerTestAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    
    // azimuth knob
    azKnob.addListener(this);
    azKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    azKnob.setBounds(50,75,150,150);
    azKnob.setRange(-180.0,180,15.0);
    azKnob.setValue(0.0);
    azKnob.setTextValueSuffix(" degrees");
    azKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 25);
    azKnob.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::ivory.darker(0.27));
    azKnob.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::limegreen.darker(0.1));
    azKnob.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::ivory.darker(0.45));
    azKnob.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colours::ivory.darker(0.33));
    addAndMakeVisible(azKnob);
    
    azKnobLabel.setText("azimuth", juce::dontSendNotification);
    azKnobLabel.attachToComponent(&azKnob, false);
    azKnobLabel.setJustificationType(juce::Justification::centredTop);
    azKnobLabel.setFont(juce::Font("Arial Black", 22.0, juce::Font::bold));
    addAndMakeVisible(azKnobLabel);
    
    
    // elevation knob
    elKnob.addListener(this);
    elKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    elKnob.setBounds(400,75,150,150);
    elKnob.setRange(-45.0,90,15.0);
    elKnob.setValue(0.0);
    elKnob.setTextValueSuffix(" degrees");
    elKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 25);
    elKnob.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::ivory.darker(0.27));
    elKnob.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::orangered.brighter(0.7));
    elKnob.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::ivory.darker(0.45));
    elKnob.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colours::ivory.darker(0.33));
    addAndMakeVisible(elKnob);
    
    elKnobLabel.setText("elevation", juce::dontSendNotification);
    elKnobLabel.attachToComponent(&elKnob, false);
    elKnobLabel.setJustificationType(juce::Justification::centredTop);
    elKnobLabel.setFont(juce::Font("Arial Black", 22.0, juce::Font::bold));
    addAndMakeVisible(elKnobLabel);
    
    
    // distance slider
    dSlider.addListener(this);
    dSlider.setSliderStyle(juce::Slider::LinearVertical);
    dSlider.setBounds(225,50,150,200);
    dSlider.setRange(2.0,14.0,0.1);
    dSlider.setValue(6.0);
    dSlider.setTextValueSuffix(" feet");
    dSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 25);
    dSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::ivory.darker(0.27));
    dSlider.setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::orange.brighter(0.9));
    dSlider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::ivory.darker(0.45));
    dSlider.setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colours::ivory.darker(0.33));
    addAndMakeVisible(dSlider);
    
    dSliderLabel.setText("distance", juce::dontSendNotification);
    dSliderLabel.attachToComponent(&dSlider, false);
    dSliderLabel.setJustificationType(juce::Justification::centredTop);
    dSliderLabel.setFont(juce::Font("Arial Black", 22.0, juce::Font::bold));
    addAndMakeVisible(dSliderLabel);
    
    
    // plugin title
    title.setText("panning, binaurally", juce::dontSendNotification);
    title.setJustificationType(juce::Justification::centred);
    title.setBounds(175,250,250,50);
    title.setFont(juce::Font("Arial Black", 32.0, juce::Font::bold));
    title.setColour(juce::Label::ColourIds::textColourId, juce::Colours::whitesmoke);
    addAndMakeVisible(title);
    
    azKnob.resized();
    elKnob.resized();
    
    setResizable(true, true);
    setResizeLimits(windowWidth/2, windowHeight/2, windowWidth*2, windowHeight*2);
    setSize(windowWidth, windowHeight);
}

BinauralPannerTestAudioProcessorEditor::~BinauralPannerTestAudioProcessorEditor()
{
}

//==============================================================================
void BinauralPannerTestAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::ivory.darker(0.28));
    
    // Define plugin bounds
    auto bounds = getLocalBounds();
    auto width = bounds.getWidth();
    auto height = bounds.getHeight();
    
    
    // Knob bounds
    auto knobWidth = width * 150/windowWidth;
    auto knobHeight = height * 150/windowHeight;
    
    auto azKnobX = width * 50/windowWidth;
    auto azKnobY = height * 75/windowHeight;
    
    auto elKnobX = width * 400/windowWidth;
    auto elKnobY = height * 75/windowHeight;
    
    azKnob.setBounds(azKnobX,azKnobY,knobWidth,knobHeight);
    elKnob.setBounds(elKnobX,elKnobY,knobWidth,knobHeight);
    
    
    // Slider bounds
    auto sliderWidth = width * 150/windowWidth;
    auto sliderHeight = height * 200/windowHeight;
    
    auto dSliderX = width * 225/windowWidth;
    auto dSliderY = height * 50/windowHeight;
    
    dSlider.setBounds(dSliderX,dSliderY,sliderWidth,sliderHeight);
    
    
    // Title bounds
    auto titleWidth = width * 250/windowWidth;
    auto titleHeight = height * 50/windowHeight;
    
    auto titleX = width * 175/windowWidth;
    auto titleY = height * 250/windowHeight;
 
    title.setBounds(titleX,titleY,titleWidth,titleHeight);
}

void BinauralPannerTestAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void BinauralPannerTestAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &azKnob)
    {
        audioProcessor.setAzimuth(azKnob.getValue());
    }
    if (slider == &elKnob)
    {
        audioProcessor.setElevation(elKnob.getValue());
    }
    if (slider == &dSlider)
    {
        audioProcessor.setDistance(dSlider.getValue());
    }
}
