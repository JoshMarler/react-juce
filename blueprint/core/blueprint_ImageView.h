#pragma once

namespace blueprint
{
    /** juce::URL::isWellFormed is currently not a complete implementation,
        so we have this slightly more robust check for now.
    */
    inline bool isWellFormedURL (const juce::URL& url)
    {
        return url.isWellFormed()
               && url.getScheme().isNotEmpty()
               && ! url.toString (false).startsWith ("data");
    }

    //==============================================================================
    /** The ImageView class is a core view for drawing images within Blueprint's layout system. */
    class ImageView final : public View
    {
    public:
        //==============================================================================
        static inline const juce::Identifier sourceProp = "source";

        //==============================================================================
        ImageView() = default;

        //==============================================================================
        void setProperty (const juce::Identifier& name, const juce::var& value) override
        {
            View::setProperty (name, value);

            if (name == sourceProp)
            {
                const auto source = value.toString();
                const auto sourceURL = juce::URL (source);

                if (isWellFormedURL (sourceURL))
                {
                    auto drawableImg = std::make_unique<juce::DrawableImage>();
                    drawableImg->setImage (loadImageFromURL (sourceURL));
                    drawable = std::move (drawableImg);
                }
                else if (source.startsWith ("data:image/"))
                {
                    // NB: juce::URL does not currently handle Data URLs
                    auto drawableImg = std::make_unique<juce::DrawableImage>();
                    drawableImg->setImage (loadImageFromDataURL (source));
                    drawable = std::move (drawableImg);
                }
                else
                {
                    // If not a URL treat source prop as inline SVG/Image data
                    drawable = juce::Drawable::createFromImageData (source.toRawUTF8(),source.getNumBytesAsUTF8());
                }
            }
        }

        //==============================================================================
        void paint (juce::Graphics& g) override
        {
            View::paint (g);

            const auto opacity = static_cast<float> (props.getWithDefault ("opacity", 1.0f));

            // Without a specified placement, we just draw the drawable.
            if (props.contains ("placement"))
            {
                // Otherwise we map placement strings to the appropriate flags
                const auto placementFlags = static_cast<int> (props["placement"]);

                drawable->drawWithin (g, getLocalBounds().toFloat(), { placementFlags }, opacity);
                return;
            }

            drawable->draw (g, opacity);
        }

    private:
        //==============================================================================
        juce::Image loadImageFromURL (const juce::URL &url) const
        {
            if (url.isLocalFile())
            {
                const auto imageFile = url.getLocalFile();

                if (! imageFile.existsAsFile())
                {
                    const auto errorString = "Image file does not exist: " + imageFile.getFullPathName();
                    jassertfalse;
                    throw std::logic_error (errorString.toStdString());
                }

                const auto image = juce::ImageFileFormat::loadFrom (imageFile);

                if (image.isNull())
                {
                    const auto errorString = "Unable to load image file: " + imageFile.getFullPathName();
                    jassertfalse;
                    throw std::logic_error (errorString.toStdString());
                }

                return image;
            }

            if (url.isProbablyAWebsiteURL(url.toString (false)))
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
                const auto errorString = "Image download not currently supported: " + url.toString (false);
                jassertfalse;
                throw std::logic_error (errorString.toStdString());
            }

            const auto errorString = "Unsupported image URL: " + url.toString (false);
            jassertfalse;
            throw std::logic_error (errorString.toStdString());
        }

        //==============================================================================
        juce::Image loadImageFromDataURL(const juce::String& source) const
        {
            // source is a data URL that describes image.
            // the format is `data:[<mediatype>][;base64],<data>`
            // https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/Data_URIs
            const auto commaIndex = source.indexOf (",");
            const auto semiIndex = source.indexOf (";");

            if (commaIndex == -1 || semiIndex == -1)
            {
                jassertfalse;
                throw std::runtime_error ("Image received an invalid data url.");
            }

            const auto base64EncodedData = source.substring (commaIndex + 1);
            juce::MemoryOutputStream outStream;

            if (! juce::Base64::convertFromBase64 (outStream, base64EncodedData))
            {
                jassertfalse;
                throw std::runtime_error ("Image failed to convert data url.");
            }

            juce::MemoryInputStream inputStream (outStream.getData(), outStream.getDataSize(), false);

            const auto mimeType = source.substring (5, semiIndex);
            auto fmt = prepareImageFormat (mimeType);

            if (fmt == nullptr)
            {
                jassertfalse;
                throw std::runtime_error ("Unsupported format.");
            }

            if (! fmt->canUnderstand(inputStream))
            {
                jassertfalse;
                throw std::runtime_error ("Cannot understand the image.");
            }

            inputStream.setPosition (0);
            return fmt->decodeImage (inputStream);
        }

        std::unique_ptr<juce::ImageFileFormat> prepareImageFormat(const juce::String& mimeType) const
        {
            if (mimeType == "image/png")    return std::make_unique<juce::PNGImageFormat>();
            if (mimeType == "image/jpeg")   return std::make_unique<juce::JPEGImageFormat>();
            if (mimeType == "image/gif")    return std::make_unique<juce::GIFImageFormat>();

            return {};
        }

        //==============================================================================
        std::unique_ptr<juce::Drawable> drawable;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ImageView)

    };
}
