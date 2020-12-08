#pragma once

namespace blueprint
{
    /** The TextView class is a core container abstraction for declaring text components
        within Blueprint's layout system.
    */
    class TextView final : public View
    {
    public:
        //==============================================================================
        /** */
        TextView() = default;

        //==============================================================================
        /** Assembles a Font from the current node properties. */
        juce::Font getFont() const
        {
            const auto fontHeight = static_cast<float> (props.getWithDefault ("font-size", 12.0f));

            juce::Font f (fontHeight);

            if (props.contains ("font-family"))
                f = juce::Font (props["font-family"], fontHeight, static_cast<int> (props.getWithDefault ("font-style", 0)));

            f.setExtraKerningFactor (props.getWithDefault ("kerning-factor", 0.0));
            return f;
        }

        /** Constructs a TextLayout from all the children string values. */
        juce::TextLayout getTextLayout (float maxWidth) const
        {
            juce::String text;

            // TODO: Right now a <Text> element maps 1:1 to a TextView instance,
            // and all children must be RawTextView instances, which are basically
            // just juce::String. A much more flexible alternative would be for a <Text>
            // element to map to a TextView and any nested raw text nodes or <Text> elements
            // map to a juce::AttributedString and carry their own properties. This allows
            // bolding single words inline, for example, and setting line-height, etc.
            for (auto& c : getChildren())
                if (auto* v = dynamic_cast<RawTextView*>(c))
                    text << v->getText();

            juce::AttributedString as (text);
            juce::TextLayout tl;

            as.setLineSpacing(props.getWithDefault("line-spacing", 1.0f));
            as.setFont(getFont());
            as.setColour(juce::Colour::fromString (props.getWithDefault ("color", "ff000000").toString()));
            as.setJustification (static_cast<int> (props.getWithDefault ("justification", 1)));

            if (props.contains ("word-wrap"))
            {
                switch (static_cast<int> (props["word-wrap"]))
                {
                    case 0:     as.setWordWrap (juce::AttributedString::WordWrap::none); break;
                    case 2:     as.setWordWrap (juce::AttributedString::WordWrap::byChar); break;
                    default:    as.setWordWrap (juce::AttributedString::WordWrap::byWord); break;
                }
            }

            tl.createLayout (as, maxWidth);
            return tl;
        }

        //==============================================================================
        /** @internal */
        void paint (juce::Graphics& g) override
        {
            const auto floatBounds = getLocalBounds().toFloat();

            View::paint (g);
            getTextLayout (floatBounds.getWidth()).draw (g, floatBounds);
        }

    private:
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TextView)
    };
}
