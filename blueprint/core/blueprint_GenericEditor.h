/*
  ==============================================================================

    blueprint_GenericEditor.h
    Created: 3 Nov 2019 4:47:39pm

  ==============================================================================
*/

#pragma once

#include "blueprint_ReactApplicationRoot.h"
#include "blueprint_ThrottleMap.h"


namespace blueprint
{

    //==============================================================================
    /** The BlueprintGenericEditor is a default AudioProcessorEditor with preinstalled functionality
     *  for working with Blueprint.
     *
     *  It automatically manages a ReactApplicationRoot, registers some native methods
     *  and properties for interfacing with the editor, and provides some helpful
     *  development tools.
     */
    class BlueprintGenericEditor
        : public juce::AudioProcessorEditor,
          public juce::AudioProcessorParameter::Listener,
          public juce::Timer
    {
    public:
        //==============================================================================
        BlueprintGenericEditor (juce::AudioProcessor*, const juce::String&, juce::AudioProcessorValueTreeState* = nullptr);
        BlueprintGenericEditor (juce::AudioProcessor*, const juce::File&, juce::AudioProcessorValueTreeState* = nullptr);

        ~BlueprintGenericEditor();

        //==============================================================================
        /** Implement the AudioProcessorParameter::Listener interface. */
        void parameterValueChanged (int parameterIndex, float newValue) override;
        void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override;

        //==============================================================================
        /** Implement the timer interface. */
        void timerCallback() override;

        /** Override the component interface. */
        void resized() override;
        void paint (juce::Graphics&) override;

    private:
        //==============================================================================
        /** Provisions and assigns a new ReactApplicationRoot. */
        void assignNewAppRoot(const juce::String&);

        /** Called for an uncaught error in the script engine.
         *
         *  Tears down an existing appRoot and paints the screen red with the
         *  stack trace printed.
         */
        void showError(const juce::String& trace);

        //==============================================================================
        std::unique_ptr<ReactApplicationRoot> appRoot;
        std::unique_ptr<juce::AttributedString> errorText;
        juce::AudioProcessorValueTreeState* valueTreeState;
        juce::File bundleFile;
        juce::Time lastModifiedTime;

        // For parameter updates to the script engine
        ThrottleMap throttleMap;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BlueprintGenericEditor)
    };

}
