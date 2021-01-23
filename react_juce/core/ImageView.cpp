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

namespace blueprint
{
    //==============================================================================
    void ImageView::setProperty(const juce::Identifier& name, const juce::var& value) 
    {
        View::setProperty(name, value);

        if (name == sourceProp)
        {
            const juce::String source = value.toString();
            const juce::URL    sourceURL = source;

            if (isWellFormedURL(sourceURL))
            {
                // Web images are downloaded in a separate thread to avoid blocking.
                if (sourceURL.isProbablyAWebsiteURL(sourceURL.toString(false)))
                {
                    const auto sourceHash = juce::DefaultHashFunctions::generateHash(source, 1024);
                    const auto& cachedImage = juce::ImageCache::getFromHashCode(sourceHash);
                    if (cachedImage.isValid())
                    {
                        setDrawableImage(cachedImage);
                    }
                    else
                    {
                        downloadImageAsync(source);
                    }
                }
                else if (sourceURL.isLocalFile())
                {
                    setDrawableImage(loadImageFromFileURL(sourceURL));
                }
                else
                {
                    const juce::String errorString = "Unsupported image URL: " + source;
                    throw std::logic_error(errorString.toStdString());
                }
            }
            else if (source.startsWith("data:image/")) // juce::URL does not currently handle Data URLs
            {
                setDrawableImage(loadImageFromDataURL(source));
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
                juce::MemoryBlock mb;

                // Did we reach the URL?
                if (!juce::URL(source).readEntireBinaryStream(mb)) 
                {
                    shouldDownloadImage = true;
                    juce::MessageManager::callAsync([this]() { sendOnErrorCallback(); });
                    return;
                }

                auto image = juce::ImageFileFormat::loadFrom(mb.getData(), mb.getSize());
                if (image.isValid()) 
                {
                    juce::MessageManager::callAsync([this, image, source]()
                    {
                        // Add the freshly downloaded image to the cache.
                        auto sourceHash = juce::DefaultHashFunctions::generateHash(source, 1024);
                        juce::ImageCache::addImageToCache(image, sourceHash);

                        // At this point, there may be multiple downloads happening at the same time,
                        // so we need to be sure the one that's just finished matches the current sourceProp.
                        if (source == props[sourceProp].toString())
                        {
                            setDrawableImage(image);
                            repaint();

                            shouldDownloadImage = false;
                        }
                    });
                }
                else
                {
                    // The URL was valid but was not pointing to a valid image.
                    juce::MessageManager::callAsync([this]() { sendOnErrorCallback(); });
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
    void ImageView::sendOnErrorCallback()
    {
        // Send an error callback to JS.
        if (props.contains(onerrorProp) && props[onerrorProp].isMethod())
        {
            std::vector<juce::var> jsArgs{ {detail::makeErrorObject("ImageViewError", "Failed to load image resource")} };
            juce::var::NativeFunctionArgs nfArgs(juce::var(), jsArgs.data(), static_cast<int>(jsArgs.size()));
            std::invoke(props[onerrorProp].getNativeFunction(), nfArgs);
        }
    }

    //==============================================================================
    void ImageView::setDrawableImage(const juce::Image& image)
    {
        auto drawableImg = std::make_unique<juce::DrawableImage>();
        drawableImg->setImage(image);
        drawable = std::move(drawableImg);

        // Notify JS that image is loaded.
        if (props.contains(onloadProp) && props[onloadProp].isMethod())
            std::invoke(props[onloadProp].getNativeFunction(), juce::var::NativeFunctionArgs(juce::var(), {}, 0));
    }

    //==============================================================================
    juce::Image ImageView::loadImageFromFileURL(const juce::URL& url) const
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
