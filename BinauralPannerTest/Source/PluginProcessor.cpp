/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BinauralPannerTestAudioProcessor::BinauralPannerTestAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), state(*this, nullptr, "PanningParams", createParameterLayout())
#endif
{
}

BinauralPannerTestAudioProcessor::~BinauralPannerTestAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout BinauralPannerTestAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    params.push_back(std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { "AzimuthAngle", 1}, // parameter ID
                                                                  "Azimuth", // parameter name in automation lane
                                                                  juce::NormalisableRange<float>(-180.f,180.f,15.0), // normalizable range
                                                                  0.f) // default value
                                                                  );
    params.push_back(std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { "ElevationAngle", 1}, // parameter ID
                                                                  "Elevation", // parameter name in automation lane
                                                                  juce::NormalisableRange<float>(-45.f,90.f,15.0), // normalizable range
                                                                  0.f) // default value
                                                                  );
    params.push_back(std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { "DistanceValue", 1}, // parameter ID
                                                                  "Distance", // parameter name in automation lane
                                                                  juce::NormalisableRange<float>(2.f,14.f,0.1), // normalizable range
                                                                  2.f) // default value
                                                                  );
    
    return { params.begin(), params.end() };
}

//==============================================================================
const juce::String BinauralPannerTestAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BinauralPannerTestAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BinauralPannerTestAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BinauralPannerTestAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BinauralPannerTestAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BinauralPannerTestAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BinauralPannerTestAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BinauralPannerTestAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String BinauralPannerTestAudioProcessor::getProgramName (int index)
{
    return {};
}

void BinauralPannerTestAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void BinauralPannerTestAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    
    // Setting up specifications
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();
    
//    interp.getHRIR(azStore, elStore, dStore, hrir);
//    conv.loadImpulseResponse(juce::AudioBuffer<float> (hrir),
//                             hrirFs,
//                             juce::dsp::Convolution::Stereo::yes,
//                             juce::dsp::Convolution::Trim::no,
//                             juce::dsp::Convolution::Normalise::no);
    
    interp.getHRIR(azStore, elStore, dStore, irBuffer);
    conv.loadImpulseResponse(juce::AudioBuffer<float>(irBuffer),
                             hrirFs,
                             juce::dsp::Convolution::Stereo::yes,
                             juce::dsp::Convolution::Trim::no,
                             juce::dsp::Convolution::Normalise::yes);

    // Seting up the convolution
    conv.prepare(spec);
    conv.reset();
}

void BinauralPannerTestAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BinauralPannerTestAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void BinauralPannerTestAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    // making audio block
    juce::dsp::AudioBlock<float> block (buffer);
    auto context = juce::dsp::ProcessContextReplacing<float> (block);
    
    // convolve audio block with the loaded impulse response
    conv.process(context);
    
    // copy to buffer
    block.copyTo(buffer);
}

// Updating IR Function
void BinauralPannerTestAudioProcessor::updateIR()
{

    // Getting azimuth, elevation, and distance
    float azimuthAngle = *state.getRawParameterValue("AzimuthAngle");
    float elevationAngle = *state.getRawParameterValue("ElevationAngle");
    float distanceValue = *state.getRawParameterValue("DistanceValue");
    
    if (azimuthAngle == -180.f) azimuthAngle = 180.f;
    if (elevationAngle == 90.f) azimuthAngle = 0.f;
    
    setAzimuth(azimuthAngle);
    setElevation(elevationAngle);
    setDistance(distanceValue);
    
    interp.getHRIR(azimuthAngle, elevationAngle, distanceValue, irBuffer);
    conv.loadImpulseResponse(juce::AudioBuffer<float>(irBuffer),
                             hrirFs,
                             juce::dsp::Convolution::Stereo::yes,
                             juce::dsp::Convolution::Trim::no,
                             juce::dsp::Convolution::Normalise::yes);
}




//==============================================================================
bool BinauralPannerTestAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BinauralPannerTestAudioProcessor::createEditor()
{
    return new BinauralPannerTestAudioProcessorEditor (*this);
}

//==============================================================================
void BinauralPannerTestAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    auto currentState = state.copyState();
    std::unique_ptr<juce::XmlElement> xml(currentState.createXml());
    copyXmlToBinary(*xml, destData);
}

void BinauralPannerTestAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(state.state.getType()))
    {
        state.replaceState(juce::ValueTree::fromXml(*xml));
    }
    
}

void BinauralPannerTestAudioProcessor::setAzimuth(float azimuthValue)
{
    azimuthAngle = azimuthValue;
}

void BinauralPannerTestAudioProcessor::setElevation(float elevationValue)
{
    elevationAngle = elevationValue;
}

void BinauralPannerTestAudioProcessor::setDistance(float distanceValue)
{
    distance = distanceValue;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BinauralPannerTestAudioProcessor();
}
