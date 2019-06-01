/*
  ==============================================================================

    blueprint_TextView.h
    Created: 28 Nov 2018 3:27:27pm

  ==============================================================================
*/

#pragma once

#include "blueprint_View.h"


namespace blueprint
{

    //==============================================================================
    /** The TextView class is a core container abstraction for declaring text components
        within Blueprint's layout system.
     */
    class TextView : public View
    {
    public:
        //==============================================================================
        TextView() = default;

        //==============================================================================
        /** Assembles a Font from the current node properties. */
        juce::Font getFont()
        {
            float fontHeight = props.getWithDefault("font-size", 12.0f);
            return juce::Font(fontHeight);
        }

        /** Assembles an aggregate string of raw text children. */
        juce::GlyphArrangement getGlyphArrangement(float maxWidth)
        {
            juce::GlyphArrangement arr;
            juce::String text;
            juce::Font f = getFont();
            juce::Justification j = juce::Justification::centredLeft;

            for (auto& c : getChildren())
                if (RawTextView* v = dynamic_cast<RawTextView*>(c))
                    text += v->getText();

            // TODO: Right now a <Text> element maps 1:1 to a TextView instance,
            // and all children must be RawTextView instances, which are basically
            // just juce::String. A much more flexible alternative would be for a <Text>
            // element to map to a TextView and any nested raw text nodes or <Text> elements
            // map to a juce::AttributedString and carry their own properties. This allows
            // bolding single words inline, for example, and setting line-height, etc.
            // Then this bit of code would accumulate the juce::AttributedText instances
            // and use juce::TextLayout to perform the actual measurement.
            if (props.contains("white-space") && props["white-space"] == "nowrap")
                arr.addLineOfText(f, text, 0, f.getHeight());
            else
                arr.addJustifiedText(f, text, 0, f.getHeight(), maxWidth, j);

            return arr;
        }

        //==============================================================================
        void paint (juce::Graphics& g) override
        {
            View::paint(g);

            juce::String hexColor = props.getWithDefault("color", "ff000000");
            juce::Colour colour = juce::Colour::fromString(hexColor);

            g.setFont(getFont());
            g.setColour(colour);

            getGlyphArrangement(cachedFloatBounds.getWidth()).draw(g);
        }

    private:
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TextView)
    };

}
