/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::String DistortionAudioProcessor::paramDrive  ("drive");
juce::String DistortionAudioProcessor::paramRange  ("range");
juce::String DistortionAudioProcessor::paramBlend  ("blend");
juce::String DistortionAudioProcessor::paramVolume  ("volume");

//==============================================================================
DistortionAudioProcessor::DistortionAudioProcessor()
: mState (*this, &mUndoManager, "Distortion",
{
    std::make_unique<juce::AudioParameterFloat>(paramDrive,
                                                TRANS ("Drive"), juce::NormalisableRange<float>(0.01f, 1.0f, 0.01f, 1.0f),
                                                mDrive.get(), "drv_val",
                                            juce::AudioProcessorParameter::genericParameter,
                                                [](float v, int) { return juce::String (v, 1); },
                                                [](const juce::String& t) { return t.dropLastCharacters (3).getFloatValue(); }),
    
    std::make_unique<juce::AudioParameterFloat>(paramRange,
                                                TRANS ("Range"), juce::NormalisableRange<float>(0.0f, 3000.0f, 0.01f, 1.0f),
                                                mRange.get(), "rng_val",
                                                juce::AudioProcessorParameter::genericParameter,
                                                [](float v, int) { return juce::String (v, 1); },
                                                [](const juce::String& t) { return t.dropLastCharacters (3).getFloatValue(); }),
    
    std::make_unique<juce::AudioParameterFloat>(paramBlend,
                                                TRANS ("Blend"),juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1.0f),
                                                mBlend.get(), "bln_val",
                                                juce::AudioProcessorParameter::genericParameter,
                                                [](float v, int) { return juce::String (v, 1); },
                                                [](const juce::String& t) { return t.dropLastCharacters (3).getFloatValue(); }),

    std::make_unique<juce::AudioParameterFloat>(paramVolume,
                                                TRANS ("Volume"), juce::NormalisableRange<float>(-72.0f, -3.0f, 0.01f, 1.0f),
                                                mVolume.get(), "dB", juce::AudioProcessorParameter::genericParameter,
                                                [](float v, int) { return juce::String (v, 1) + " dB"; },
                                                [](const juce::String& t) { return t.dropLastCharacters (3).getFloatValue(); })
})//NormalisableRange (ValueType rangeStart, ValueType rangeEnd, ValueType intervalValue, ValueType skewFactor, bool useSymmetricSkew=false) noexcept

{
    mState.addParameterListener (paramDrive, this);
    mState.addParameterListener (paramRange, this);
    mState.addParameterListener (paramBlend, this);
    mState.addParameterListener (paramVolume, this);
}

DistortionAudioProcessor::~DistortionAudioProcessor()
{
}

//==============================================================================
const juce::String DistortionAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DistortionAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool DistortionAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool DistortionAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double DistortionAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DistortionAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int DistortionAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DistortionAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DistortionAudioProcessor::getProgramName (int index)
{
    return {};
}

void DistortionAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}


//==============================================================================
void DistortionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void DistortionAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void DistortionAudioProcessor::parameterChanged (const juce::String &parameterID, float newValue)
{
    if (parameterID == paramDrive) {
        mDrive = newValue;
    }
    else if (parameterID == paramRange) {
        mRange = newValue;
    }
    else if (parameterID == paramBlend) {
        mBlend = newValue;
    }
    else if (parameterID == paramVolume) {
        mVolume = newValue;
    }
}

bool DistortionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // we only support stereo and mono
    if (layouts.getMainInputChannels() == 0 || layouts.getMainInputChannels() > 2)
        return false;
    
    if (layouts.getMainOutputChannels() == 0 || layouts.getMainOutputChannels() > 2)
        return false;
    
    // we don't allow the narrowing the number of channels
    if (layouts.getMainInputChannels() > layouts.getMainOutputChannels())
        return false;
    
    return true;
}


void DistortionAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
    
    //parameters
    const float drive = mDrive.get();
    const float range = mRange.get();
    const float blend = mBlend.get();
    const float volume = juce::Decibels::decibelsToGain (mVolume.get());
    
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        
        for (int sample = 0; sample < buffer.getNumSamples(); sample++) //why not ++sample here?
        {
            //a parameter for clean data to mix with the processed channelData
            float cleanData = *channelData;
            
            *channelData *= drive * range;
            
            //distortion via atan //HOW CAN WE USE DIFFERENT DISTORTIONS IN THE SAME PLUG-IN?
            *channelData = ((((2.f / juce::float_Pi) * atan(*channelData)) * blend) + (cleanData * 1.f-blend)) * volume;
            
            channelData++;
        }
    }
}

//THIS WAS MISSING!!!
juce::AudioProcessorValueTreeState& DistortionAudioProcessor::getValueTreeState()
{
    return mState;
}


//==============================================================================
bool DistortionAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DistortionAudioProcessor::createEditor()
{
    return new DistortionAudioProcessorEditor (*this);
}

//==============================================================================
void DistortionAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DistortionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DistortionAudioProcessor();
}
