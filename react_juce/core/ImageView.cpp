/*
  ==============================================================================

    blueprint_ImageView.cpp
    Created: 13 Jan 2021 10:54pm

  ==============================================================================
*/

#include "ImageView.h"


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
                    downloadImageAsync(source);
                }
                else if (sourceURL.isLocalFile())
                {
                    auto drawableImg = std::make_unique<juce::DrawableImage>();
                    drawableImg->setImage(loadImageFromFileURL(sourceURL));
                    drawable = std::move(drawableImg);
                }
                else
                {
                    const juce::String errorString = "Unsupported image URL: " + source;
                    throw std::logic_error(errorString.toStdString());
                }
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
        // Add a thread to the thread pool on ReactApplicationRoot
        // The class listens to the download and the onFinish() callback will be called
        // when download is finished.

        // Allow only one download at the same time for a given ImageView.
        if (imageDownloader != nullptr && imageDownloader->isRunning())
            return;

        if (auto* appRoot = findParentComponentOfClass<ReactApplicationRoot>())
        {
            appRoot->addDownloadJob([this, source] ()
            {
                imageDownloader.reset(new ImageDownloader(source));
                // Image has been downloaded successfully, we can assign the drawable.
                imageDownloader->onSuccess = [this](juce::Image& image)
                {
                    juce::MessageManager::callAsync([this, image]() 
                        {
                            auto drawableImg = std::make_unique<juce::DrawableImage>();
                            drawableImg->setImage(image);
                            drawable = std::move(drawableImg);
                            repaint();

                            shouldDownloadImage = false;
                        }
                    );
                };
                // Manage the error case.
                imageDownloader->onError = [this]()
                {
                    // Call an onError prop function ?
                    shouldDownloadImage = true;
                };

                imageDownloader->start();
            });
        }
        else
        {
            shouldDownloadImage = true;
        }
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