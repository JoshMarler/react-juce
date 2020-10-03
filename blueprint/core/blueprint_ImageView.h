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

                if (source.startsWith("data:image/"))
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
            // source is a data URL that describes image.
            // the format is `data:[<mediatype>][;base64],<data>`
            // https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/Data_URIs
            int commaIndex = source.indexOf(",");
            int semiIndex = source.indexOf(";");
            if (commaIndex == -1 || semiIndex == -1)
            {
                throw std::runtime_error("Image received an invalid data url.");
            }
            
            auto base64EncodedData = source.substring(commaIndex + 1);
            juce::MemoryOutputStream outStream{};

            if(!juce::Base64::convertFromBase64(outStream, base64EncodedData))
            {
                throw std::runtime_error("Image failed to convert data url.");
            }

            juce::MemoryInputStream inputStream (outStream.getData(), outStream.getDataSize(), false);

            auto mimeType = source.substring(5,semiIndex);
            auto fmt = prepareImageFormat(mimeType);
            
            if (fmt == nullptr)
            {
                throw std::runtime_error("Unsupported format.");
            }

            if (!fmt->canUnderstand(inputStream))
            {
                throw std::runtime_error("Cannot understand the image.");
            }
            
            inputStream.setPosition(0);
            return fmt->decodeImage(inputStream);
        }
        
        std::unique_ptr<juce::ImageFileFormat> prepareImageFormat(juce::String& mimeType)
        {
            if (mimeType == "image/png")
            {
                return std::make_unique<juce::PNGImageFormat>();
            }
            
            if (mimeType == "image/jpeg")
            {
                return std::make_unique<juce::JPEGImageFormat>();
            }
            
            if (mimeType == "image/gif")
            {
                return std::make_unique<juce::GIFImageFormat>();
            }
            return nullptr;
        }
    };

}
