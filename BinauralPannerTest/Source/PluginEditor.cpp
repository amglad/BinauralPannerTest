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
    tree.addListener(this);
    addMouseListener(this, false);
    setInterceptsMouseClicks(true, false);
    
    tree = audioProcessor.state.state;
    
    valueTreeRedirected(audioProcessor.state.state);
    
    // azimuth knob
    azKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    azKnob.setRotaryParameters(-3.14f, 3.14f, true);
    azKnob.setBounds(50,75,150,150);
    azKnob.setRange(-180.0,180,10.0); // Changed
    azKnob.setValue(0.0);
    azKnob.setTextValueSuffix(" degrees");
    azKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 25);
    azKnob.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::ivory.darker(0.27));
    azKnob.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::ivory.darker(0.33));
    azKnob.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::limegreen.darker(0.1));
    azKnob.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colours::ivory.darker(0.33));
    addAndMakeVisible(azKnob);
    
    azKnobLabel.setText("azimuth", juce::dontSendNotification);
    azKnobLabel.attachToComponent(&azKnob, false);
    azKnobLabel.setJustificationType(juce::Justification::centredTop);
    azKnobLabel.setFont(juce::Font("Arial Black", 22.0, juce::Font::bold));
    addAndMakeVisible(azKnobLabel);
    
    
    // elevation knob
    elKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    elKnob.setRotaryParameters(-2.36f, 0.f, true); // Need to change?
    elKnob.setBounds(400,75,150,150);
    elKnob.setRange(-90.0,90,10.0); // Changed
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
    dSlider.setSliderStyle(juce::Slider::LinearVertical);
    dSlider.setBounds(225,50,150,200);
    dSlider.setRange(0.1,1.0,0.01); // Changed
    dSlider.setValue(0.1); // Changed
    dSlider.setTextValueSuffix(" meters");
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
    dSlider.resized();
    
    setResizable(true, true);
    setResizeLimits(windowWidth/2, windowHeight/2, windowWidth*2, windowHeight*2);
    setSize(windowWidth, windowHeight);
    
    sliderAttachment.emplace_back(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.state, "AzimuthAngle", azKnob));
    sliderAttachment.emplace_back(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.state, "ElevationAngle", elKnob));
    sliderAttachment.emplace_back(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.state, "DistanceValue", dSlider));
    
 //   headTop = juce::ImageCache::getFromMemory(BinaryData::outlinetop2_png, BinaryData::outlinetop2_pngSize);
 //   headSide = juce::ImageCache::getFromMemory(BinaryData::outlineside_png, BinaryData::outlineside_pngSize);
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
    
  //  g.drawImageWithin(headTop, azKnobX, azKnobY, knobWidth, knobHeight, juce::RectanglePlacement::Flags (juce::RectanglePlacement::centred));
  //  g.drawImageWithin(headSide, elKnobX, elKnobY, knobWidth/2, knobHeight/2, juce::RectanglePlacement::Flags (juce::RectanglePlacement::onlyReduceInSize),(juce::RectanglePlacement::centred));
 //   g.drawImage(headSide, elKnobX, elKnobY, knobWidth, knobHeight, elKnobX, elKnobY, knobWidth/2, knobHeight/2);
    
    
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


void BinauralPannerTestAudioProcessorEditor::valueTreePropertyChanged (juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property)
{
    audioProcessor.updateIR();
}


void BinauralPannerTestAudioProcessorEditor::valueTreeRedirected (juce::ValueTree &)
{
//    if (juce::MessageManager::getInstance()->isThisTheMessageThread())
//        int test = 1;
//    else
//        int test = 1;
}
