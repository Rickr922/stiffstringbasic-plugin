/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
StiffStringAudioProcessorEditor::StiffStringAudioProcessorEditor (StiffStringAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
	setSize(400, 300);

	mPlayButton.addListener(this);
	mStopButton.addListener(this);

	mPlayButton.setClickingTogglesState(false);
	mStopButton.setClickingTogglesState(false);
}

StiffStringAudioProcessorEditor::~StiffStringAudioProcessorEditor()
{
	mPlayButton.removeListener(this);
	mStopButton.removeListener(this);
}

//==============================================================================
void StiffStringAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void StiffStringAudioProcessorEditor::resized()
{
	int vButtonWidth = 80;
	int vButtonHeight = 25;
	int vButtonY = 150;
	int vCenter = (int)getWidth() / 2;
	mPlayButton.setButtonText("PLAY");
	mPlayButton.setBounds(vCenter - vButtonWidth - 5, vButtonY, vButtonWidth, vButtonHeight);
	mStopButton.setBounds(vCenter + 5, vButtonY, vButtonWidth, vButtonHeight);
	mStopButton.setButtonText("STOP");
	addAndMakeVisible(mPlayButton);
	addAndMakeVisible(mStopButton);
}

void StiffStringAudioProcessorEditor::buttonClicked(juce::Button* apButton)
{
	if (apButton == &mPlayButton)
	{
		audioProcessor.SetPlayState(true);
	}
	else if (apButton == &mStopButton)
	{
		audioProcessor.SetPlayState(false);
	}
}
