/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class GainPluginAudioProcessorEditor
    : public AudioProcessorEditor,
      public AudioProcessorParameter::Listener,
      public Timer
{
public:
    GainPluginAudioProcessorEditor (GainPluginAudioProcessor&);
    ~GainPluginAudioProcessorEditor();

    //==============================================================================
    void parameterValueChanged (int parameterIndex, float newValue) override;
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override {}
    void timerCallback() override;

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GainPluginAudioProcessor& processor;
    blueprint::ReactApplicationRoot appRoot;

    //==============================================================================
    // The plugin editor holds an array of parameter value readouts which are
    // propagated to the user interface. During parameter value changes on the
    // realtime thread, we capture the values in this array of structs, then at
    // 30Hz propagate the value changes via dispatching events to the jsui.
    struct ParameterReadout {
        std::atomic<float> value = 0.0;
        std::atomic<bool> dirty = false;

        ParameterReadout() = default;

        ParameterReadout(const ParameterReadout& other) {
            value = other.value.load();
            dirty = other.dirty.load();
        }
    };

    std::vector<ParameterReadout> paramReadouts;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GainPluginAudioProcessorEditor)
};
