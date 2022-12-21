/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class StiffStringAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    StiffStringAudioProcessor();
    ~StiffStringAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    void SetPlayState(bool aPlayState);
    float Limiter(const float aValue);


private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StiffStringAudioProcessor)

    float mStringLength{ 0.7f };
    int mPointLength{ 0 };
    float mC{ 160.f };
    float mStiffness{ 0.015f };
    float mSigma0{ 0.5f };
    float mSigma1{ 0.005f };

    float mStringCoeff1{ 0.f };
    float mStringCoeff2{ 0.f };
    float mStringCoeff3{ 0.f };
    float mStringCoeff4{ 0.f };
    float mStringCoeff5{ 0.f };

    double mTimeStep = { 0.0 };
    double mSpaceStep = { 0.0 };
  
    //std::atomic<float> mFrequency{ 0.f };
    std::atomic<bool> mPlayState{ false };

    std::vector<std::vector<float>> mStringStates;
    std::vector<float*> mpStatesPointers;

    int mOutPos{ 0 };
    int mInPos{ 0 };

    //==============================================================================
    void ResetSpaceStep();
    void ResetStringCoeffs();
    void ResetStringStates();
    void ExciteString();
    void UpdateStates();
};
