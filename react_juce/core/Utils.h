/*
  ==============================================================================

    Utils.h
    Created: 20 Jan 2021 10:44:27pm

  ==============================================================================
*/

#pragma once

namespace reactjuce
{
    namespace detail
    {
        // Constructs a generic error object to pass through to JS
        juce::var makeErrorObject(const juce::String& errorName, const juce::String& errorMessage);
    }
}
