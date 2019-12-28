/*
  ==============================================================================

    blueprint_HotRealoader.h
    Created: 11 Dec 2019 2:20:29pm

  ==============================================================================
*/

#pragma once

#include "blueprint_ReactApplicationRoot.h"

namespace blueprint {
//==============================================================================
/** The BlueprintGenericEditor is a default AudioProcessorEditor with preinstalled functionality
     *  for working with Blueprint.
     *
     *  It automatically manages a ReactApplicationRoot, registers some native methods
     *  and properties for interfacing with the editor, and provides some helpful
     *  development tools.
     */
    class HotReloader : public juce::Timer {
    public:
        HotReloader(std::unique_ptr<blueprint::ReactApplicationRoot> &appRoot, const juce::File &bundle,
                    juce::Component &parentJuceComp);

        ~HotReloader();

        void timerCallback() override;

    private:
        //==============================================================================
        /** Provisions and assigns a new ReactApplicationRoot. */
        void assignNewAppRoot(const juce::String &codePath);

        std::unique_ptr<blueprint::ReactApplicationRoot> &root;
        juce::File bundleFile;

        juce::Time lastModifiedTime;
        juce::Component &parentComp;
    };
} // namespace blueprint
