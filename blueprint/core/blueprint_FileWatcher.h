/*
  ==============================================================================

    blueprint_FileWatcher.h
    Created: 10 Oct 2020 3:08:39pm

  ==============================================================================
*/

#pragma once


namespace blueprint
{

    //==============================================================================
    /** Helper class which watches files for changes and triggers a user supplied
     *  callback in thte event of a file change.
     */
    class FileWatcher : private juce::Timer
    {
    public:
        using FileChangedCallback = std::function<void(void)>;

        explicit FileWatcher (FileChangedCallback && callback)
            : onFileChanged(std::move(callback)) {}

        void start() { startTimer(50); }
        void stop() { stopTimer(); }

        void watch (const juce::File& f)
        {
            jassert(f.existsAsFile());

            // If we're already watching that bundle, nothing to do
            if (watchedFiles.count(f) > 0)
                return;

            watchedFiles.emplace(f, f.getLastModificationTime());
        }

        std::vector<juce::File> getWatchedFiles()
        {
            std::vector<juce::File> ret;

            for (const auto& pair : watchedFiles)
                ret.push_back(pair.first);

            return ret;
        }

    private:
        void timerCallback() override
        {
            bool shouldFireChangeEvent = false;

            // First step is to erase any files that no longer exist from our
            // file map, and check for any changes along the way
            for (auto it = watchedFiles.begin(); it != watchedFiles.end();)
            {
                auto& f = (*it).first;
                bool const fileExists = f.existsAsFile() && (f.getSize() > 0);

                if (!fileExists)
                {
                    it = watchedFiles.erase(it);
                    continue;
                }

                const auto lmt = f.getLastModificationTime();

                if (lmt > (*it).second)
                {
                    shouldFireChangeEvent = true;
                    watchedFiles[f] = lmt;
                }

                it++;
            }

            // Now we fire once if any bundle has changed, which coalesces changes
            // into a single event in response to which we should re-evaluate
            // all watched files.
            if (shouldFireChangeEvent)
            {
                onFileChanged();
            }
        }

        // It's important here that we're using a std::map because we want the
        // insertion order retained. In the event of a file change, we want to
        // re-evaluate all bundles within the cleaned engine in the same order
        // we started with.
        std::map<juce::File, juce::Time>            watchedFiles;
        FileChangedCallback                         onFileChanged;
    };

}
