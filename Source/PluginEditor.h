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
class StiffStringAudioProcessorEditor  
    : public juce::AudioProcessorEditor
    , public juce::Button::Listener
{
public:
    StiffStringAudioProcessorEditor (StiffStringAudioProcessor&);
    ~StiffStringAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    //==============================================================================
    void buttonClicked(juce::Button* apButton) override;

private:
    StiffStringAudioProcessor& audioProcessor;

    juce::TextButton mPlayButton;
    juce::TextButton mStopButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StiffStringAudioProcessorEditor)
};
