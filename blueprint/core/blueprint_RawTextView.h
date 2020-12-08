#pragma once

namespace blueprint
{
    /** The RawTextView class is more or less a wrapper around a string that asserts
        itself as a leaf node in the tree by refusing a resize and painting nothing.

        It provides an interface for a parent TextView to draw a collection of
        RawTextView children. For example,

        `<Text>123 Avenue Avenue {'\n'} City, St 12345</Text>`

        creates three RawTextView children of a single TextView. But independently
        these raw text strings can't know how to lay themselves out correctly.
     */
    class RawTextView final : public View
    {
    public:
        //==============================================================================
        RawTextView (const juce::String& initialText) : text (initialText) {}

        //==============================================================================
        /** */
        void setText (const juce::String& newText) { text = newText; }

        /** */
        const juce::String& getText() const noexcept { return text; }

        //==============================================================================
        /** @internal */
        void setProperty (const juce::Identifier&, const juce::var&) override
        {
            jassertfalse;
            throw std::logic_error ("A RawTextView can't receive properties.");
        }

    private:
        //==============================================================================
        juce::String text;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RawTextView)
    };

}
