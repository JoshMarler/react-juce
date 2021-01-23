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
        static juce::var makeErrorObject(const juce::String& errorName, const juce::String& errorMessage)
        {
            auto* o = new juce::DynamicObject();

            o->setProperty("name", errorName);
            o->setProperty("message", errorMessage);

            return juce::var(o);
        }
    }
}
