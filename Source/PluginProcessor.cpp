/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
StiffStringAudioProcessor::StiffStringAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

StiffStringAudioProcessor::~StiffStringAudioProcessor()
{
    /*for (int i = 0; i < 3; ++i)
    {
        delete mpStatesPointers[i];
    }*/
}

//==============================================================================
const juce::String StiffStringAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool StiffStringAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool StiffStringAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool StiffStringAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double StiffStringAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int StiffStringAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int StiffStringAudioProcessor::getCurrentProgram()
{
    return 0;
}

void StiffStringAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String StiffStringAudioProcessor::getProgramName (int index)
{
    return {};
}

void StiffStringAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void StiffStringAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    mTimeStep = 1 / sampleRate;
    ResetSpaceStep();
}

void StiffStringAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool StiffStringAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void StiffStringAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    //auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();  
    float vOutput;
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        if (mPlayState)
        {
            for (int j = 2; j < mPointLength - 2; ++j)
            {
                mpStatesPointers[0][j] = mpStatesPointers[1][j] * mStringCoeff1
                    + (mpStatesPointers[1][j + 1] + mpStatesPointers[1][j - 1]) * mStringCoeff2
                    - (mpStatesPointers[1][j + 2] + mpStatesPointers[1][j - 2]) * mStringCoeff3
                    + mpStatesPointers[2][j] * mStringCoeff4
                    - (mpStatesPointers[2][j + 1] + mpStatesPointers[2][j - 1]) * mStringCoeff5;
            }

            vOutput = Limiter(mpStatesPointers[0][mOutPos]);
            UpdateStates();
            //DBG(mpStringStateNext.get()[mInPos]);
        }
        else
        {
            vOutput = 0;
        }
        for (int channel = 0; channel < totalNumOutputChannels; ++channel)
        {
            auto vpBuffer = buffer.getWritePointer(channel);
            vpBuffer[i] = vOutput;
        }
    }
}

//==============================================================================
bool StiffStringAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* StiffStringAudioProcessor::createEditor()
{
    return new StiffStringAudioProcessorEditor (*this);
}

//==============================================================================
void StiffStringAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void StiffStringAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
void StiffStringAudioProcessor::SetPlayState(bool aPlayState)
{
    if (!mPlayState.load())
    {
        if (aPlayState)
        {
            ResetStringStates();
            ExciteString();
            mPlayState.store(aPlayState);
        }
    }
    else
    {
        if (!aPlayState)
        {
            mPlayState.store(aPlayState);
            ResetStringStates();
        }
        else
        {
            ExciteString();
        }
    }
}

float StiffStringAudioProcessor::Limiter(const float aValue)
{
    if (aValue > 1) return 1;
    else if (aValue < -1) return -1;
    return aValue;
}

//==============================================================================
void StiffStringAudioProcessor::ResetSpaceStep()
{
    auto vCoeff1 = (float)(mC * mC * mTimeStep * mTimeStep + 4.f * mSigma1 * mTimeStep);
    auto vCoeff2 = sqrt(vCoeff1 * vCoeff1 + 16 * mTimeStep * mTimeStep * mStiffness * mStiffness);
    mSpaceStep = sqrt(vCoeff1 + vCoeff2);
    mPointLength = static_cast<int>(mStringLength / mSpaceStep);
    //mSpaceStep = mPointLength / mStringLength;
    mOutPos = (int)mPointLength / 2;
    mInPos = (int)mPointLength / 3;

    ResetStringCoeffs();
    ResetStringStates();
}

void StiffStringAudioProcessor::ResetStringCoeffs()
{
    float vDen = (mSigma0 * mTimeStep + 1.f);
    auto vCoeff1 = 2.f / vDen;
    auto vCoeff2 = (mSigma0 * mTimeStep - 1.f) / vDen;
    auto vCoeff3 = (float)((mC * mC * mTimeStep * mTimeStep + 2.f * mSigma1 * mTimeStep) / (mSpaceStep * mSpaceStep * vDen));
    auto vCoeff4 = (float)(mStiffness * mStiffness * mTimeStep * mTimeStep / (mSpaceStep * mSpaceStep * mSpaceStep * mSpaceStep * vDen));
    auto vCoeff5 = (float)(2.f * mSigma1 * mTimeStep / (mSpaceStep * mSpaceStep * vDen));

    mStringCoeff1 = (vCoeff1 - 2 * vCoeff3 - 6 * vCoeff4);
    mStringCoeff2 = (vCoeff3 + 4 * vCoeff4);
    mStringCoeff3 = vCoeff4;
    mStringCoeff4 = (vCoeff2 + 2 * vCoeff5);
    mStringCoeff5 = vCoeff5;
}

void StiffStringAudioProcessor::ResetStringStates()
{
    mStringStates = std::vector<std::vector<float>>(3,std::vector<float>(mPointLength, 0));
    mpStatesPointers.resize(3, nullptr);
    for (int i = 0; i < 3; ++i)
    {
        mpStatesPointers[i] = &mStringStates[i][0];
    }
}

void StiffStringAudioProcessor::ExciteString()
{
    mpStatesPointers[1][mInPos] = 1;
}

void StiffStringAudioProcessor::UpdateStates()
{
    // Do a pointer-switch. MUCH quicker than copying two entire state vectors every time-step.
    auto vpTempPointer = mpStatesPointers[2];
    mpStatesPointers[2] = mpStatesPointers[1];
    mpStatesPointers[1] = mpStatesPointers[0];
    mpStatesPointers[0] = vpTempPointer;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StiffStringAudioProcessor();
}
