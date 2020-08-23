/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
/** Helper function for generating the parameter layout. */
AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    AudioProcessorValueTreeState::ParameterLayout params (
        std::make_unique<AudioParameterFloat>(
            "MainGain",
            "Gain",
            NormalisableRange<float>(0.0, 1.0),
            0.8,
            String(),
            AudioProcessorParameter::genericParameter,
            [](float value, int maxLength) {
                return String(Decibels::gainToDecibels(value), 1) + "dB";
            },
            nullptr
        )
    );

    return params;
}

//==============================================================================
GainPluginAudioProcessor::GainPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                       .withOutput ("Output", AudioChannelSet::stereo(), true)),
       params(*this, nullptr, JucePlugin_Name, createParameterLayout())
{
}

GainPluginAudioProcessor::~GainPluginAudioProcessor()
{
}

//==============================================================================
const String GainPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GainPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GainPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GainPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GainPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GainPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GainPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GainPluginAudioProcessor::setCurrentProgram (int index)
{
}

const String GainPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void GainPluginAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void GainPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    gain.reset(sampleRate, 0.02);
}

void GainPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GainPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void GainPluginAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
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

    // Our intense dsp processing
    gain.setValue(*params.getRawParameterValue("MainGain"));
    gain.applyGain(buffer, buffer.getNumSamples());

    // We'll also report peak values for our meter. This isn't an ideal way to do this
    // as the rate between the audio processing callback and the timer on which the
    // editor reads these values could mean missing peaks in the visual display, but
    // this is a simple example plugin so let's not worry about it.
    lcPeak.store(buffer.getMagnitude(0, 0, buffer.getNumSamples()));
    rcPeak.store(buffer.getMagnitude(1, 0, buffer.getNumSamples()));
}

//==============================================================================
bool GainPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* GainPluginAudioProcessor::createEditor()
{
    // The GainPlugin example has two different editors available to demonstrate
    // different approaches. The first is a familiar approach using the
    // AudioProcessorEditor file that the Projucer scaffolding tool sets up for you,
    // with some manual effort to load the javascript bundle and install some custom
    // hooks for the plugin (such as reporting gain meter values). Uncomment the line
    // below to enable that approach.
    // return new GainPluginAudioProcessorEditor (*this);

    // The second example uses the BlueprintGenericEditor, which is a default
    // AudioProcessorEditor included in Blueprint that will automatically bootstrap
    // your React root, install some native method hooks for parameter interaction
    // if you provide an AudioProcessorValueTreeState, and manage hot reloading
    // of the source bundle. You can always start with the BlueprintGenericEditor
    // then switch to a custom editor when you need more explicit control.
    File sourceDir = File(__FILE__).getParentDirectory();
    File bundle = sourceDir.getChildFile("jsui/build/js/main.js");

    auto* editor = new blueprint::BlueprintGenericEditor(*this, bundle, &params);

    editor->setResizable(true, true);
    editor->setResizeLimits(400, 240, 400 * 2, 240 * 2);
    editor->getConstrainer()->setFixedAspectRatio(400.0 / 240.0);
    editor->setSize (400, 240);

    return editor;
}

//==============================================================================
void GainPluginAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void GainPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GainPluginAudioProcessor();
}
