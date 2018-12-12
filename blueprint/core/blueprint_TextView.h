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
    /** We use this method to measure the size of a given string so that the Text container
        knows what size to take.
     */
    YGSize measureTextNode(YGNodeRef, float, YGMeasureMode, float, YGMeasureMode);

    //==============================================================================
    /** The TextView class is a core container abstraction for declaring text components
        within Blueprint's layout system.
     */
    class TextView : public View
    {
    public:
        //==============================================================================
        TextView()
        {
            YGNodeSetContext(yogaNode, this);
            YGNodeSetMeasureFunc(yogaNode, measureTextNode);
        }

        //==============================================================================
        void setProperty (const juce::Identifier& name, const juce::var& newValue) override
        {
            View::setProperty(name, newValue);

            if (name == juce::Identifier("textValue"))
                YGNodeMarkDirty(yogaNode);
        }

        //==============================================================================
        juce::Font getFont()
        {
            auto& props = getProperties();
            float fontHeight = props.getWithDefault("font-size", 12.0f);
            return juce::Font(fontHeight);
        }

        juce::String getTextValue()
        {
            auto& props = getProperties();
            return props.getWithDefault("textValue", "");
        }

        //==============================================================================
        void paint (juce::Graphics& g) override
        {
            View::paint(g);

            auto& props = getProperties();
            juce::String colorValue = props.getWithDefault("color", "ff000000");
            juce::Colour colour = juce::Colour::fromString(colorValue);
            juce::Justification just = juce::Justification::centredLeft;

            g.setFont(getFont());
            g.setColour(colour);
            g.drawText(getTextValue(), getLocalBounds(), just, true);
        }

    private:
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TextView)
    };

}
