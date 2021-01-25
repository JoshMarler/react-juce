/*
  ==============================================================================

    GenericEditor.h
    Created: 3 Nov 2019 4:47:39pm

  ==============================================================================
*/

#pragma once

#include "AppHarness.h"
#include "ReactApplicationRoot.h"


namespace reactjuce
{

    //==============================================================================
    /** The Editor is a default AudioProcessorEditor with preinstalled functionality
     *  for working with React.
     *
     *  It automatically manages a ReactApplicationRoot, registers some native methods
     *  and properties for interfacing with the editor, and provides some helpful
     *  development tools.
     */
    class GenericEditor
        : public juce::AudioProcessorEditor
        , public juce::AudioProcessorParameter::Listener
        , public juce::Timer
    {
    public:
        //==============================================================================
        GenericEditor (juce::AudioProcessor&, const juce::File&);
        ~GenericEditor() override;

        //==============================================================================
        /** Implement the AudioProcessorParameter::Listener interface. */
        void parameterValueChanged (int parameterIndex, float newValue) override;
        void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override;

        //==============================================================================
        /** Override the timer interface. */
        void timerCallback() override;

        //==============================================================================
        /** Override the component interface. */
        void resized() override;
        void paint (juce::Graphics&) override;
        
        /** Public getter to access appRoot instance */
        ReactApplicationRoot& getReactAppRoot() { return appRoot; }

    private:
        //==============================================================================
        /** ReactApplicationRoot bundle eval callback functions */
        void beforeBundleEvaluated();
        void afterBundleEvaluated();

        //==============================================================================
        std::shared_ptr<EcmascriptEngine>     engine;
        ReactApplicationRoot                  appRoot;
        AppHarness                            harness;

        juce::File                            bundleFile;

        // We keep a map of the parameter IDs and their associated parameter pointers
        // to have a quick lookup in beforeBundleEvaluated where lambdas are called
        // with a param ID
        std::map<juce::String, juce::AudioProcessorParameter*> parameters;

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
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GenericEditor)
    };

}
