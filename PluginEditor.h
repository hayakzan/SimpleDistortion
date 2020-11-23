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
class DistortionAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    DistortionAudioProcessorEditor (DistortionAudioProcessor&);
    ~DistortionAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
 
private:
    DistortionAudioProcessor& processor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionAudioProcessorEditor)
    
    juce::Slider mDriveSlider      { juce::Slider::RotaryHorizontalVerticalDrag,  juce::Slider::TextBoxBelow };
    juce::Slider mRangeSlider      { juce::Slider::RotaryHorizontalVerticalDrag,  juce::Slider::TextBoxBelow };
    juce::Slider mBlendSlider      { juce::Slider::RotaryHorizontalVerticalDrag,  juce::Slider::TextBoxBelow };
    juce::Slider mVolumeSlider  { juce::Slider::RotaryHorizontalVerticalDrag,  juce::Slider::TextBoxBelow };
    
    juce::AudioProcessorValueTreeState::SliderAttachment mDriveAttachment      { processor.getValueTreeState(), DistortionAudioProcessor::paramDrive, mDriveSlider };
    juce::AudioProcessorValueTreeState::SliderAttachment mRangeAttachment      { processor.getValueTreeState(), DistortionAudioProcessor::paramRange, mRangeSlider };
    juce::AudioProcessorValueTreeState::SliderAttachment mBlendAttachment      { processor.getValueTreeState(), DistortionAudioProcessor::paramBlend, mBlendSlider };
    juce::AudioProcessorValueTreeState::SliderAttachment mVolumeAttachment  { processor.getValueTreeState(), DistortionAudioProcessor::paramVolume, mVolumeSlider };
};
