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
        juce::GlyphArrangement getGlyphArrangement()
        {
            juce::GlyphArrangement arr;
            juce::String text;
            juce::Font f = getFont();
            juce::Justification j = juce::Justification::centredLeft;
            int maxWidth = props.getWithDefault("max-width", INT_MAX);

            for (auto& c : getChildren())
                if (RawTextView* v = dynamic_cast<RawTextView*>(c))
                    text += v->getText();

            arr.addJustifiedText(f, text, 0, f.getHeight(), maxWidth, j);
            arr.justifyGlyphs(0, arr.getNumGlyphs(), 0, 0, getWidth(), getHeight(), j);

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

            getGlyphArrangement().draw(g);
        }

    private:
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TextView)
    };

}
