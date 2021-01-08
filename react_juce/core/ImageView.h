/*
  ==============================================================================

    blueprint_ImageView.h
    Created: 20 Apr 2019 5:25:25pm

  ==============================================================================
*/

#pragma once

#include "View.h"


namespace blueprint
{
    namespace
    {
        // juce::URL::isWellFormed is currently not a complete
        // implementation, so we have this slightly more robust check
        // for now.
        bool isWellFormedURL(const juce::URL &url)
        {
            return url.isWellFormed()  &&
                   url.getScheme().isNotEmpty() &&
                   !url.toString(false).startsWith("data");
        }

    }

    //==============================================================================
    /** The ImageView class is a core view for drawing images within Blueprint's
        layout system.
     */
    class ImageView : public View
    {
    public:
        //==============================================================================
        static inline juce::Identifier sourceProp    = "source";
        static inline juce::Identifier placementProp = "placement";

        //==============================================================================
        ImageView() = default;

        //==============================================================================
        void setProperty (const juce::Identifier& name, const juce::var& value) override
        {
            View::setProperty(name, value);

            if (name == sourceProp)
            {
                const juce::String source    = value.toString();
                const juce::URL    sourceURL = source;

                if (isWellFormedURL(sourceURL))
                {
                    auto drawableImg = std::make_unique<juce::DrawableImage>();
                    drawableImg->setImage(loadImageFromURL(sourceURL));
                    drawable = std::move(drawableImg);
                }
                else if (source.startsWith("data:image/")) // juce::URL does not currently handle Data URLs
                {
                    auto drawableImg = std::make_unique<juce::DrawableImage>();
                    drawableImg->setImage(loadImageFromDataURL(source));
                    drawable = std::move(drawableImg);
                }
                else // If not a URL treat source prop as inline SVG/Image data
                {
                    drawable = std::unique_ptr<juce::Drawable>(
                            juce::Drawable::createFromImageData(
                                    source.toRawUTF8(),
                                    source.getNumBytesAsUTF8()
                            )
                    );
                }
            }
        }

        //==============================================================================
        void paint (juce::Graphics& g) override
        {
            View::paint(g);

            const float opacity = props.getWithDefault(opacityProp, 1.0f);

            // Without a specified placement, we just draw the drawable.
            if (!props.contains(placementProp))
                return drawable->draw(g, opacity);

            // Otherwise we map placement strings to the appropriate flags
            const int existingFlags = props[placementProp];
            const juce::RectanglePlacement placement (existingFlags);

            drawable->drawWithin(g, getLocalBounds().toFloat(), placement, opacity);
        }

    private:
        //==============================================================================
        juce::Image loadImageFromURL(const juce::URL &url) const
        {
            if (url.isLocalFile())
            {
                const juce::File imageFile = url.getLocalFile();

                if (!imageFile.existsAsFile())
                {
                    const juce::String errorString = "Image file does not exist: " + imageFile.getFullPathName();
                    throw std::logic_error(errorString.toStdString());
                }

                juce::Image image = juce::ImageFileFormat::loadFrom(imageFile);

                if (image.isNull())
                {
                    const juce::String errorString = "Unable to load image file: " + imageFile.getFullPathName();
                    throw std::logic_error(errorString.toStdString());
                }

                return image;
            }

            if (url.isProbablyAWebsiteURL(url.toString(false)))
            {
                //TODO: What approach should we take here?
                //      It looks like using juce::URL::downloadToFile would be the best
                //      option as this handles nerwork reconnects etc. However, some
                //      posts on the juce forum suggest that you're best not to call this
                //      from the message thread as it can block whilst connecting to the server
                //      (before launching the download task on a separate thread). So, we could either
                //      use Thread::launch to trigger url.downloadFile() or we could use a ThreadPool
                //      with a job if we think there are usecases for frequent image downloads etc.
                //      ThreadPool might be the best option here and we can initialise it with a single
                //      thread for now and increase if this becomes necessary. Finally, should we consider
                //      some sort of "placeHolder" prop on <Image> which can be used to display a string
                //      or child View until the image is downloaded?
                const juce::String errorString = "Image download not currently supported: " + url.toString(false);
                throw std::logic_error(errorString.toStdString());
            }

            const juce::String errorString = "Unsupported image URL: " + url.toString(false);
            throw std::logic_error(errorString.toStdString());
        }

        //==============================================================================
        juce::Image loadImageFromDataURL(const juce::String& source) const
        {
            // source is a data URL that describes image.
            // the format is `data:[<mediatype>][;base64],<data>`
            // https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/Data_URIs
            const int commaIndex = source.indexOf(",");
            const int semiIndex = source.indexOf(";");

            if (commaIndex == -1 || semiIndex == -1)
            {
                throw std::runtime_error("Image received an invalid data url.");
            }

            const auto base64EncodedData = source.substring(commaIndex + 1);
            juce::MemoryOutputStream outStream{};

            if(!juce::Base64::convertFromBase64(outStream, base64EncodedData))
            {
                throw std::runtime_error("Image failed to convert data url.");
            }

            juce::MemoryInputStream inputStream (outStream.getData(), outStream.getDataSize(), false);

            const auto mimeType = source.substring(5,semiIndex);
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

        std::unique_ptr<juce::ImageFileFormat> prepareImageFormat(const juce::String& mimeType) const
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

        //==============================================================================
        std::unique_ptr<juce::Drawable> drawable;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ImageView)

    };
}
