#include "PluginProcessor.h"
#include "PluginEditor.h"

STS_Sim_2DGuidanceAudioProcessor::STS_Sim_2DGuidanceAudioProcessor()
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
    startTimer(timer_Interval_MS);

    for (int i = 0; i < 8; i++)
        sawtooths[i] = (double) i / 8.0;
}

STS_Sim_2DGuidanceAudioProcessor::~STS_Sim_2DGuidanceAudioProcessor()
{
}

void STS_Sim_2DGuidanceAudioProcessor::hiResTimerCallback()
{
    update_Ang_Dist();
    updateErrors();
    updateFVs();
    if (isReady) mapToFAUST();

    // UPDATE TIME AND CHECK SENSOR STATUS
    timeDone += (double)timer_Interval_MS / 1000;
    cyclesDone++;

    timeSinceStateChange += (double)timer_Interval_MS / 1000.0;

    if (isTrial_ON)
    {
        timeElapsed_presentTrial += (double)timer_Interval_MS / 1000.0;
        if (isRecording_Trials)
        {
            timeStamps.push_back(timeElapsed_presentTrial);
            user_Traj_X.push_back(userPos_X);
            user_Traj_Y.push_back(userPos_Y);
            series_inTargetBox.push_back(user_inTargetBox ? 1 : 0);
            user_angX.push_back(userPos_ANG_X);
        }
    }

    // UPDATE TRIAL STATUS AND SAVE IF NEEDED
    if ((isTrial_ON_Z1 && !isTrial_ON) && isRecording_Trials)
    {
        saveTrialFile();
    }

    isTrial_ON_Z1 = isTrial_ON;
}

const juce::String STS_Sim_2DGuidanceAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool STS_Sim_2DGuidanceAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool STS_Sim_2DGuidanceAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool STS_Sim_2DGuidanceAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double STS_Sim_2DGuidanceAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int STS_Sim_2DGuidanceAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int STS_Sim_2DGuidanceAudioProcessor::getCurrentProgram()
{
    return 0;
}

void STS_Sim_2DGuidanceAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String STS_Sim_2DGuidanceAudioProcessor::getProgramName (int index)
{
    return {};
}

void STS_Sim_2DGuidanceAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void STS_Sim_2DGuidanceAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    fDSP = new mydsp();
    fDSP->init(sampleRate);
    fUI = new MapUI();
    fDSP->buildUserInterface(fUI);
    outputs = new float* [2];
    for (int channel = 0; channel < 2; ++channel) {
        outputs[channel] = new float[samplesPerBlock];
    }
    isReady = true;
    stopSound();
}

void STS_Sim_2DGuidanceAudioProcessor::releaseResources()
{
    if (isDSP_ON)
        stopMusicDSP();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool STS_Sim_2DGuidanceAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void STS_Sim_2DGuidanceAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    if (isReady)
    {
        fDSP->compute(buffer.getNumSamples(), NULL, outputs);                           // GET MUSIC FROM FAUST
        //triggerVoice = movementAnalysis.voice_isTrigger;

        for (int channel = 0; channel < totalNumOutputChannels; ++channel) {
            for (int i = 0; i < buffer.getNumSamples(); i++) {
                *buffer.getWritePointer(channel, i) = outputs[channel][i]; //+ outputVoice; // SUM DATA
            }

            //audio_preLoaded.trigger_z1 = triggerVoice;

            // GET DB LEVEL FOR METERING
            musicLevel_dB = fmax(-50, Decibels::gainToDecibels(buffer.getMagnitude(0, 0, buffer.getNumSamples())));
        }
    }
}

//==============================================================================
bool STS_Sim_2DGuidanceAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* STS_Sim_2DGuidanceAudioProcessor::createEditor()
{
    return new STS_Sim_2DGuidanceAudioProcessorEditor (*this);
}

//==============================================================================
void STS_Sim_2DGuidanceAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void STS_Sim_2DGuidanceAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new STS_Sim_2DGuidanceAudioProcessor();
}
