/*
  ==============================================================================

    blueprint_ImageView.h
    Created: 20 Apr 2019 5:25:25pm

  ==============================================================================
*/

#pragma once

#include "View.h"
#include "ImageDownloader.h"


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
        static inline juce::Identifier onloadProp    = "onLoad";

        //==============================================================================
        ImageView() = default;

        //==============================================================================
        void setProperty(const juce::Identifier& name, const juce::var& value) override;
        void paint(juce::Graphics& g) override;
        void parentHierarchyChanged() override;
        //==============================================================================
    private:
        //==============================================================================
        void downloadImageAsync(const juce::String& source);
        juce::Image loadImageFromFileURL(const juce::URL& url) const;
        juce::Image loadImageFromDataURL(const juce::String& source) const;
        std::unique_ptr<juce::ImageFileFormat> prepareImageFormat(const juce::String& mimeType) const;

        //==============================================================================
        std::unique_ptr<juce::Drawable> drawable;

        std::unique_ptr<ImageDownloader> imageDownloader;
        bool shouldDownloadImage{ false };

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ImageView)
    };
}
