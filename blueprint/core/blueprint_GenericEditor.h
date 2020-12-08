#pragma once

namespace blueprint
{
    /** The BlueprintGenericEditor is a default AudioProcessorEditor with preinstalled functionality
        for working with Blueprint.

        It automatically manages a ReactApplicationRoot, registers some native methods
        and properties for interfacing with the editor, and provides some helpful
        development tools.
    */
    class BlueprintGenericEditor final : public juce::AudioProcessorEditor,
                                         public juce::AudioProcessorParameter::Listener,
                                         private juce::Timer
    {
    public:
        //==============================================================================
        BlueprintGenericEditor (juce::AudioProcessor&, const juce::File&, juce::AudioProcessorValueTreeState* = nullptr);
        ~BlueprintGenericEditor() override;

        //==============================================================================
        void parameterValueChanged (int parameterIndex, float newValue) override;
        void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override;
        void resized() override;
        void paint (juce::Graphics&) override;

    private:
        //==============================================================================
        std::shared_ptr<EcmascriptEngine> engine { std::make_shared<EcmascriptEngine>() };
        ReactApplicationRoot appRoot;
        AppHarness harness;

        juce::File bundleFile;
        juce::AudioProcessorValueTreeState* valueTreeState = nullptr;

        //==============================================================================
        // The plugin editor holds an array of parameter value readouts which are
        // propagated to the user interface. During parameter value changes on the
        // realtime thread, we capture the values in this array of structs, then at
        // 30Hz propagate the value changes via dispatching events to the jsui.
        struct ParameterReadout
        {
            ParameterReadout() = default;

            ParameterReadout (const ParameterReadout& other) :
                value (other.value.load()),
                dirty (other.dirty.load())
            {
            }

            std::atomic<float> value { 0.0f };
            std::atomic<bool> dirty { false };
        };

        juce::Array<ParameterReadout> paramReadouts;

        //==============================================================================
        void beforeBundleEvaluated();
        void afterBundleEvaluated();

        void timerCallback() override;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BlueprintGenericEditor)
    };
}
