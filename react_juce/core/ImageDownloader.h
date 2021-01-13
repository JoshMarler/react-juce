/*
  ==============================================================================

    blueprint_ImageDownloader.h
    Created: 13 Jan 2021 11:02am

  ==============================================================================
*/

#pragma once

namespace blueprint
{
    class ImageDownloader : public juce::URL::DownloadTask::Listener
    {
    public:
        ImageDownloader(const juce::String& source) : downloadUrl(source) { }

        void start()
        {
            running = true;
            juce::File tmpImage = juce::File::createTempFile("tmp");
            if (tmpImage.create()) {
                std::unique_ptr<juce::URL::DownloadTask> downloadTask = downloadUrl.downloadToFile(tmpImage, juce::String(), this);
                if (downloadTask)
                {
                    // Keep the thread alive...
                    while (downloadTask->isFinished() == false)
                    {
                        juce::Thread::sleep(200);
                    }
                }
                else
                {
                    running = false;
                }
            }
        }

        // Callback from the asynchronous download.
        void finished(juce::URL::DownloadTask* task, bool success) override
        {
            juce::Image image;
            if (success)
            {
                juce::File imageFile = task->getTargetLocation();
                if (imageFile.existsAsFile())
                {
                    image = juce::ImageFileFormat::loadFrom(imageFile);
                    if (!image.isNull() && onSuccess != nullptr) onSuccess(image);

                    imageFile.deleteFile();
                }
            }
            if (image.isNull() && onError != nullptr) onError();

            running = false;
        }

        bool isRunning() const
        {
            return running;
        }

        std::function<void(juce::Image& image)> onSuccess = nullptr;
        std::function<void()> onError = nullptr;
    private:
        juce::URL downloadUrl;
        bool running{ false };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImageDownloader)
    };
}