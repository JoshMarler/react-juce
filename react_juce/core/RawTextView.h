/*
  ==============================================================================

    RawTextView.h
    Created: 11 Apr 2019 11:27:27am

  ==============================================================================
*/

#pragma once

#include "View.h"


namespace reactjuce
{

    //==============================================================================
    /** The RawTextView class is more or less a wrapper around a string that asserts
        itself as a leaf node in the tree by refusing a resize and painting nothing.

        It provides an interface for a parent TextView to draw a collection of
        RawTextView children. For example,

            `<Text>123 Avenue Avenue {'\n'} City, St 12345</Text>`

        creates three RawTextView children of a single TextView. But independently
        these raw text strings can't know how to lay themselves out correctly.
     */
    class RawTextView : public View
    {
    public:
        //==============================================================================
        RawTextView(const juce::String& text) : _text(text) {}

        //==============================================================================
        void setProperty (const juce::Identifier&, const juce::var&) override
        {
            throw std::logic_error("A RawTextView can't receive properties.");
        }

        //==============================================================================
        void setText (const juce::String& text) {
            _text = text;
        }

        juce::String getText()
        {
            return _text;
        }

    private:
        //==============================================================================
        juce::String _text;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RawTextView)
    };

}
