/*
  ==============================================================================

    ImageView.cpp
    Created: 13 Jan 2021 10:54pm

  ==============================================================================
*/

#include "ImageView.h"
#include "Utils.h"

namespace
{
    // juce::URL::isWellFormed is currently not a complete
    // implementation, so we have this slightly more robust check
    // for now.
    bool isWellFormedURL(const juce::URL& url)
    {
        return url.isWellFormed() &&
            url.getScheme().isNotEmpty() &&
            !url.toString(false).startsWith("data");
    }

}

namespace reactjuce
{
    //==============================================================================
    void ImageView::setProperty(const juce::Identifier& name, const juce::var& value)
    {
        View::setProperty(name, value);

        if (name == sourceProp)
        {
            const juce::String source = value.toString();
            const int sourceHash      = juce::DefaultHashFunctions::generateHash(source, 1024);

            // No need to recalculate the image if it's already in the ImageCache.
            const juce::Image& cachedImage = juce::ImageCache::getFromHashCode(sourceHash);
            if (cachedImage.isValid())
                return setDrawableImage(cachedImage, sourceHash);

            try
            {
                const juce::URL sourceURL = source;

                // Web images are downloaded in a separate thread to avoid blocking.
                if (isWellFormedURL(sourceURL) && sourceURL.isProbablyAWebsiteURL(sourceURL.toString(false)))
                    return downloadImageAsync(source);

                if (sourceURL.isLocalFile())
                {
                    const juce::File imageFile = sourceURL.getLocalFile();

                    if (imageFile.getFileExtension() == ".svg")
                        return setDrawableSVG(imageFile);
                    else
                        return setDrawableImage(loadImageFromFile(imageFile), sourceHash);
                }

                if (source.startsWith("data:image/")) // juce::URL does not currently handle Data URLs
                    return setDrawableImage(loadImageFromDataURL(source), sourceHash);

                // Last case left, raw Image data.
                return setDrawableData(source);
            }
            catch (const std::exception& l)
            {
                // Every image format can throw an exception that we catch here
                // to send an onError callback to the React component.
                return sendOnErrorCallback(juce::String(l.what()));
            }
        }
    }

    //==============================================================================
    void ImageView::paint(juce::Graphics& g)
    {
        View::paint(g);

        if (drawable == nullptr) return;

        const float opacity = props.getWithDefault(opacityProp, 1.0f);

        // Without a specified placement, we just draw the drawable.
        if (!props.contains(placementProp))
            return drawable->draw(g, opacity);

        // Otherwise we map placement strings to the appropriate flags
        const int existingFlags = props[placementProp];
        const juce::RectanglePlacement placement(existingFlags);

        drawable->drawWithin(g, getLocalBounds().toFloat(), placement, opacity);
    }

    //==============================================================================
    void ImageView::sendOnLoadCallback()
    {
        // Notify JS that image is loaded.
        if (props.contains(onloadProp) && props[onloadProp].isMethod())
            std::invoke(props[onloadProp].getNativeFunction(), juce::var::NativeFunctionArgs(juce::var(), {}, 0));
    }

    //==============================================================================
    void ImageView::sendOnErrorCallback(const juce::String& message)
    {
        // Send an error callback to JS.
        if (props.contains(onerrorProp) && props[onerrorProp].isMethod())
        {
            std::vector<juce::var> jsArgs{ {detail::makeErrorObject("ImageViewError", message)} };
            juce::var::NativeFunctionArgs nfArgs(juce::var(), jsArgs.data(), static_cast<int>(jsArgs.size()));
            std::invoke(props[onerrorProp].getNativeFunction(), nfArgs);
        }
    }

    //==============================================================================
    void ImageView::setDrawableImage(const juce::Image& image, const int sourceHash)
    {
        // Add the freshly retrieved image to the cache.
        juce::ImageCache::addImageToCache(image, sourceHash);

        auto drawableImg = std::make_unique<juce::DrawableImage>();
        drawableImg->setImage(image);
        drawable = std::move(drawableImg);

        repaint();
        sendOnLoadCallback();
    }

    void ImageView::setDrawableSVG(const juce::File& svgFile)
    {
        if (!svgFile.existsAsFile())
        {
            const juce::String errorString = "SVG file does not exist: " + svgFile.getFullPathName();
            throw std::logic_error(errorString.toStdString());
        }

        drawable = std::unique_ptr<juce::Drawable>(
                juce::Drawable::createFromSVGFile(svgFile));

        if (drawable == nullptr)
        {
            const juce::String errorString = "Invalid SVG file: " + svgFile.getFullPathName();
            throw std::logic_error(errorString.toStdString());
        }

        repaint();
        sendOnLoadCallback();
    }

    void ImageView::setDrawableData(const juce::String& source)
    {
        // If not a URL treat source prop as inline SVG/Image data
        drawable = std::unique_ptr<juce::Drawable>(
            juce::Drawable::createFromImageData(
                source.toRawUTF8(),
                source.getNumBytesAsUTF8()
            )
        );

        if (drawable == nullptr)
        {
            const juce::String errorString = "Unsupported image URL: " + source;
            throw std::logic_error(errorString.toStdString());
        }

        repaint();
        sendOnLoadCallback();
    }

    //==============================================================================
    void ImageView::parentHierarchyChanged()
    {
        if (shouldDownloadImage)
        {
            downloadImageAsync(props[sourceProp].toString());
        }
    }

    //==============================================================================
    void ImageView::downloadImageAsync(const juce::String& source)
    {
        if (auto* appRoot = findParentComponentOfClass<ReactApplicationRoot>())
        {
            appRoot->getThreadPool().addJob([this, source] ()
            {
                shouldDownloadImage = false;
                juce::MemoryBlock mb;

                // Did we reach the URL?
                if (!juce::URL(source).readEntireBinaryStream(mb))
                {
                    juce::MessageManager::callAsync([this]() {
                        sendOnErrorCallback("Could not reach URL");
                    });
                    return;
                }

                auto image = juce::ImageFileFormat::loadFrom(mb.getData(), mb.getSize());
                if (image.isValid())
                {
                    juce::MessageManager::callAsync([this, image, source]()
                    {
                        // At this point, there may be multiple downloads happening at the same time,
                        // so we need to be sure the one that's just finished matches the current sourceProp.
                        if (source == props[sourceProp].toString())
                        {
                            auto sourceHash = juce::DefaultHashFunctions::generateHash(source, 1024);
                            setDrawableImage(image, sourceHash);
                        }
                    });
                }
                else
                {
                    // The URL was valid but was not pointing to a valid image.
                    juce::MessageManager::callAsync([this]() {
                        sendOnErrorCallback("The URL was not pointing to a valid image");
                    });
                }
            });
        }
        else
        {
            // It will be called later on parentHierarchyChanged.
            shouldDownloadImage = true;
        }
    }

    //==============================================================================
    juce::Image ImageView::loadImageFromFile(const juce::File& imageFile) const
    {
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

    //==============================================================================
    juce::Image ImageView::loadImageFromDataURL(const juce::String& source) const
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

        if (!juce::Base64::convertFromBase64(outStream, base64EncodedData))
        {
            throw std::runtime_error("Image failed to convert data url.");
        }

        juce::MemoryInputStream inputStream(outStream.getData(), outStream.getDataSize(), false);

        const auto mimeType = source.substring(5, semiIndex);
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

    std::unique_ptr<juce::ImageFileFormat> ImageView::prepareImageFormat(const juce::String& mimeType) const
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
}
