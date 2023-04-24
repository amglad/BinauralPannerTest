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
//    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
//        buffer.clear (i, 0, buffer.getNumSamples());
//    // might need these?
//    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
//        contextBuffer.clear (i, 0, buffer.getNumSamples());
//    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
//        contextStoreBuffer.clear (i, 0, buffer.getNumSamples());
    
    int numSamples = buffer.getNumSamples();
    
    
    // Getting azimuth, elevation, and distance
    float azimuthAngle = *state.getRawParameterValue("AzimuthAngle");
    float elevationAngle = *state.getRawParameterValue("ElevationAngle");
    float distanceValue = *state.getRawParameterValue("DistanceValue");
    
    if (azimuthAngle == -180.f) azimuthAngle = 180.f;
    if (elevationAngle == 90.f) azimuthAngle = 0.f;
    
    setAzimuth(azimuthAngle);
    setElevation(elevationAngle);
    setDistance(distanceValue);
    
    // Making Audio Block
    juce::dsp::AudioBlock<float> block (buffer);
    auto context = juce::dsp::ProcessContextReplacing<float> (block);
    
    double hrirFs = 96000;
    int numSamplesHRIR = 2048;
    
    // Getting the proper hrir when a knob moves
    if (azimuthAngle != azStore || elevationAngle != elStore || distanceValue != dStore)
    {
        
        juce::dsp::AudioBlock<float> blockStore (buffer);
        auto contextStore = juce::dsp::ProcessContextReplacing<float> (blockStore);
        // convolve and put it in a buffer for crossfading
        interp.getHRIR(azStore, elStore, dStore, hrirStore);
        conv.loadImpulseResponse(juce::AudioBuffer<float> (hrir),
                                 hrirFs,
                                 juce::dsp::Convolution::Stereo::yes,
                                 juce::dsp::Convolution::Trim::yes,
                                 juce::dsp::Convolution::Normalise::no);
        conv.process(contextStore);
        blockStore.copyTo(contextStoreBuffer);

        interp.getHRIR(azimuthAngle, elevationAngle, distanceValue, hrir);
        conv.loadImpulseResponse(juce::AudioBuffer<float> (hrir),
                                 hrirFs,
                                 juce::dsp::Convolution::Stereo::yes,
                                 juce::dsp::Convolution::Trim::yes,
                                 juce::dsp::Convolution::Normalise::no);
        conv.process(context);
        block.copyTo(contextBuffer);
        
        i = 0;
    }
        
    if (i >= 0 && i <= numSamplesHRIR/numSamples)
    {
        for (int c = 0; c < totalNumOutputChannels; ++c)
        {
            for (int n = 0; n < numSamples; ++n)
            {
                buffer.getWritePointer(c)[n] = contextStoreBuffer.getWritePointer(c)[n + (numSamples * i)] +
                                                     contextBuffer.getWritePointer(c)[n + (numSamples * i)];
            }
        }

        i++;
    }
    else{
        // Writing to block
        conv.process(context);
        // Writing to buffer
        block.copyTo(buffer);
    }
    
    // Storing comparison values
    azStore = azimuthAngle;
    elStore = elevationAngle;
    dStore = distanceValue;
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
