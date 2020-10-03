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

                if (source.startsWith("data:"))
                {
                    auto drawableImg = std::make_unique<juce::DrawableImage>();
                    const juce::Image img = loadImageFromDataURL(source);
                    drawableImg->setImage(img);
                    drawable = std::move(drawableImg);
                    return;
                }

                // handle svg image
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

        //==============================================================================
        juce::Image loadImageFromDataURL(juce::String& source)
        {
            int index = source.indexOf(",");
            juce::String base64Encoded = source.substring(index + 1);
            juce::MemoryOutputStream outStream{};

            if(!juce::Base64::convertFromBase64(outStream, base64Encoded))
            {
                throw std::logic_error("Image failed to convert data url.");
            }

            std::unique_ptr<juce::InputStream> inputStream;
            inputStream.reset(new juce::MemoryInputStream(outStream.getData(), outStream.getDataSize(), false));

            auto fmt = std::make_unique<juce::PNGImageFormat>();
            return fmt->decodeImage(*inputStream.get());
        }
    };

}
