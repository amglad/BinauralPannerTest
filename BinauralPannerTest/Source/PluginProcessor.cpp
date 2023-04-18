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
                                                                  juce::NormalisableRange<float>(-165.f,180.f,15.0), // normalizable range
                                                                  0.f) // default value
                                                                  );
    params.push_back(std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { "ElevationAngle", 1}, // parameter ID
                                                                  "Elevation", // parameter name in automation lane
                                                                  juce::NormalisableRange<float>(-45.f,75.f,15.0), // normalizable range
                                                                  0.f) // default value
                                                                  );
    params.push_back(std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { "DistanceValue", 1}, // parameter ID
                                                                  "Distance", // parameter name in automation lane
                                                                  juce::NormalisableRange<float>(2.f,14.f,0.1), // normalizable range
                                                                  6.f) // default value
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
    juce::dsp::ProcessSpec spec;
    
    // Setting up specifications
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();
    
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

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
        
    
    
    // Getting buffer samples
    int numSamples = buffer.getNumSamples();
    
    // Getting azimuth, elevation, and distance
    float azimuthAngle = *state.getRawParameterValue("AzimuthAngle");
    setAzimuth(azimuthAngle);
    float elevationAngle = *state.getRawParameterValue("ElevationAngle");
    setElevation(elevationAngle);
    float distanceValue = *state.getRawParameterValue("DistanceValue");
    setDistance(distanceValue);
    
    // Making Audio Block
    juce::dsp::AudioBlock<float> block {buffer};
    
    // Doing for both channels
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        // Getting the proper hrir
        if (azimuthAngle != azStore || elevationAngle != elStore || distanceValue != dStore)
        {
            hrir = interp.getHRIR(azimuthAngle, elevationAngle, distanceValue, numSamples, channel);
            // Loading the impulse response we want to the convolution
            conv.loadImpulseResponse(hrir, 2048, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::yes, 0);
        }
        
        // Setting comparison values
        azStore = azimuthAngle;
        elStore = elevationAngle;
        dStore = distanceValue;
        
        // Writing to block
        conv.process(juce::dsp::ProcessContextReplacing<float>(block));
        // Writing to buffer
        block.copyTo(buffer);
        
    }

    
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    
    
    
    

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
