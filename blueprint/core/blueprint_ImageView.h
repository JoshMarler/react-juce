/*
  ==============================================================================

    blueprint_ImageView.h
    Created: 20 Apr 2019 5:25:25pm

  ==============================================================================
*/

#pragma once

#include "blueprint_View.h"


namespace blueprint
{

    //==============================================================================
    /** The ImageView class is a core view for drawing images within Blueprint's
        layout system.
     */
    class ImageView : public View
    {
    public:
        //==============================================================================
        ImageView() = default;

        //==============================================================================
        void setProperty (const juce::Identifier& name, const juce::var& value) override
        {
            View::setProperty(name, value);

            if (name == juce::Identifier("source"))
            {
                juce::String source = value.toString();

                drawable = std::unique_ptr<juce::Drawable>(
                    juce::Drawable::createFromImageData(
                        source.toRawUTF8(),
                        source.getNumBytesAsUTF8()
                    )
                );
            }
        }

        //==============================================================================
        void paint (juce::Graphics& g) override
        {
            View::paint(g);

            float opacity = props.getWithDefault("opacity", 1.0f);

            // Without a specified placement, we just draw the drawable.
            if (!props.contains("placement"))
                return drawable->draw(g, opacity);

            // Otherwise we map placement strings to the appropriate flags
            int flags = props["placement"];
            juce::RectanglePlacement placement (flags);

            drawable->drawWithin(g, getLocalBounds().toFloat(), placement, opacity);
        }

    private:
        //==============================================================================
        std::unique_ptr<juce::Drawable> drawable;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ImageView)
    };

}
