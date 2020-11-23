/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DistortionAudioProcessorEditor::DistortionAudioProcessorEditor (DistortionAudioProcessor& p)
: AudioProcessorEditor (&p), processor (p)
{
    addAndMakeVisible (mDriveSlider);
    addAndMakeVisible (mRangeSlider);
    addAndMakeVisible (mBlendSlider);
    addAndMakeVisible (mVolumeSlider);
    
    setSize (400, 250);
}

DistortionAudioProcessorEditor::~DistortionAudioProcessorEditor()
{
}

//==============================================================================
void DistortionAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkgoldenrod);
    
    g.setColour (juce::Colours::silver);
    g.setFont   (24.0f);
    
    auto box = getLocalBounds().reduced (20);
    box = box.withTop (box.getBottom() - 40);
    
    const auto width = box.getWidth() / 4;
    g.drawFittedText (TRANS ("Drive"), box.removeFromLeft (width), juce::Justification::centred, 1);
    g.drawFittedText (TRANS ("Range"), box.removeFromLeft (width), juce::Justification::centred, 1);
    g.drawFittedText (TRANS ("Blend"), box.removeFromLeft (width), juce::Justification::centred, 1);
    g.drawFittedText (TRANS ("Volume"), box.removeFromLeft (width), juce::Justification::centred, 1);
}

void DistortionAudioProcessorEditor::resized()
{
    auto box = getLocalBounds().reduced (20);
    box.removeFromBottom (40);
    
    const auto width = box.getWidth() / 4;
    mDriveSlider.setBounds (box.removeFromLeft (width).reduced (10));
    mRangeSlider.setBounds (box.removeFromLeft (width).reduced (10));
    mBlendSlider.setBounds (box.removeFromLeft (width).reduced (10));
    mVolumeSlider.setBounds (box.removeFromLeft (width).reduced (10));
}
