/*
  ==============================================================================

    TextView.h
    Created: 28 Nov 2018 3:27:27pm

  ==============================================================================
*/

#pragma once

#include "View.h"


namespace reactjuce
{

    //==============================================================================
    /** The TextView class is a core container abstraction for declaring text components
        within the layout system.
     */
    class TextView : public View
    {
    public:
        //==============================================================================
        static const inline juce::Identifier colorProp         = "color";

        static const inline juce::Identifier fontSizeProp      = "font-size";
        static const inline juce::Identifier fontStyleProp     = "font-style";
        static const inline juce::Identifier fontFamilyProp    = "font-family";

        static const inline juce::Identifier justificationProp = "justification";
        static const inline juce::Identifier kerningFactorProp = "kerning-factor";
        static const inline juce::Identifier lineSpacingProp   = "line-spacing";
        static const inline juce::Identifier wordWrapProp      = "word-wrap";

        //==============================================================================
        TextView() = default;

        //==============================================================================
        /** Assembles a Font from properties. */
        static juce::Font getFont(const juce::NamedValueSet &fontProps)
        {
            const float fontHeight = fontProps.getWithDefault(fontSizeProp, 12.0f);
            const int textStyleFlags = fontProps.getWithDefault(fontStyleProp, 0);

            juce::Font f (fontHeight);

            if (fontProps.contains(fontFamilyProp))
                f = juce::Font (fontProps[fontFamilyProp], fontHeight, textStyleFlags);

            f.setExtraKerningFactor(fontProps.getWithDefault(kerningFactorProp, 0.0));
            return f;
        }

        /** Assembles a Font from the current node properties. */
        juce::Font getFont()
        {
            return getFont(props);
        }

        /** Constructs a TextLayout from all the children string values. */
        juce::TextLayout getTextLayout (float maxWidth)
        {
            juce::String hexColor = props.getWithDefault(colorProp, "ff000000");
            juce::Colour colour = juce::Colour::fromString(hexColor);
            int just = props.getWithDefault(justificationProp, 1);
            juce::String text;

            // TODO: Right now a <Text> element maps 1:1 to a TextView instance,
            // and all children must be RawTextView instances, which are basically
            // just juce::String. A much more flexible alternative would be for a <Text>
            // element to map to a TextView and any nested raw text nodes or <Text> elements
            // map to a juce::AttributedString and carry their own properties. This allows
            // bolding single words inline, for example, and setting line-height, etc.
            for (auto& c : getChildren())
                if (RawTextView* v = dynamic_cast<RawTextView*>(c))
                    text += v->getText();

            juce::AttributedString as (text);
            juce::TextLayout tl;

            as.setLineSpacing(props.getWithDefault(lineSpacingProp, 1.0f));
            as.setFont(getFont());
            as.setColour(colour);
            as.setJustification(just);

            if (props.contains(wordWrapProp))
            {
                int wwValue = props[wordWrapProp];

                switch (wwValue)
                {
                    case 0:
                        as.setWordWrap(juce::AttributedString::WordWrap::none);
                        break;
                    case 2:
                        as.setWordWrap(juce::AttributedString::WordWrap::byChar);
                        break;
                    case 1:
                    default:
                        as.setWordWrap(juce::AttributedString::WordWrap::byWord);
                        break;

                }
            }

            tl.createLayout(as, maxWidth);
            return tl;
        }

        //==============================================================================
        void paint (juce::Graphics& g) override
        {
            auto floatBounds = getLocalBounds().toFloat();

            View::paint(g);
            getTextLayout(floatBounds.getWidth()).draw(g, floatBounds);
        }

    private:
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TextView)
    };

}
