/*
  ==============================================================================

    Utils.cpp
    Created: 20 Jan 2021 10:44:27pm

  ==============================================================================
*/

#include "Utils.h"

namespace reactjuce
{
    namespace detail
    {
        juce::var makeErrorObject(const juce::String& errorName, const juce::String& errorMessage)
        {
            juce::DynamicObject::Ptr o = new juce::DynamicObject();

            o->setProperty("name", errorName);
            o->setProperty("message", errorMessage);

            return o.get();
        }
    }
}
