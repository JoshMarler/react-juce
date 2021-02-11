/*
  ==============================================================================

    Utils.h
    Created: 20 Jan 2021 10:44:27pm

  ==============================================================================
*/

#pragma once

#include <variant>


namespace reactjuce
{
    namespace detail
    {
        // Constructs a generic error object to pass through to JS
        juce::var makeErrorObject(const juce::String& errorName, const juce::String& errorMessage);

        // Constructs either a Color or ColorGradient
        std::variant<juce::Colour, juce::ColourGradient> makeColorVariant(const juce::var& colorVariant, const juce::Rectangle<int>& localBounds);
    }
}
