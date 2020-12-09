#pragma once

namespace blueprint
{
    /** Helper class which watches files for changes and triggers a user supplied
        callback in thte event of a file change.
    */
    class FileWatcher final : private juce::Timer
    {
    public:
        //==============================================================================
        /** */
        using FileChangedCallback = std::function<void (void)>;

        //==============================================================================
        /** */
        explicit FileWatcher (FileChangedCallback&& callback)
            : onFileChanged(std::move(callback)) {}

        //==============================================================================
        /** */
        void start() { startTimer (50); }

        /** */
        void stop() { stopTimer(); }

        /** */
        void watch (const juce::File& f)
        {
            JUCE_ASSERT_MESSAGE_THREAD

            jassert (f.existsAsFile());

            // If we're already watching that bundle, nothing to do
            if (watchedFiles.count (f) > 0)
                return;

            watchedFiles.emplace (f, f.getLastModificationTime());
        }

        /** */
        juce::Array<juce::File> getWatchedFiles() const
        {
            juce::Array<juce::File> ret;
            ret.resize (static_cast<int> (watchedFiles.size()));

            for (const auto& pair : watchedFiles)
                ret.addIfNotAlreadyThere (pair.first);

            return ret;
        }

    private:
        //==============================================================================
        /** It's important here that we're using a std::map because we want the
            insertion order retained. In the event of a file change, we want to
            re-evaluate all bundles within the cleaned engine in the same order
            we started with.
        */
        std::map<juce::File, juce::Time> watchedFiles;

        FileChangedCallback onFileChanged;

        //==============================================================================
        void timerCallback() override
        {
            bool shouldFireChangeEvent = false;

            // First step here is to be careful for empty files. Some bundlers
            // may delete the target file just before replacing it with their
            // new compiled result, and if we happen to poll in between we'll
            // get a messed up result.
            for (auto it = watchedFiles.begin(); it != watchedFiles.end(); ++it)
            {
                auto& f = (*it).first;

                if (f.existsAsFile() && f.getSize() > 0)
                {
                    const auto lmt = f.getLastModificationTime();

                    if (lmt > (*it).second)
                    {
                        shouldFireChangeEvent = true;
                        watchedFiles[f] = lmt;
                    }
                }
            }

            // Now we fire once if any bundle has changed, which coalesces changes
            // into a single event in response to which we should re-evaluate all watched files.
            if (shouldFireChangeEvent)
                onFileChanged();
        }

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FileWatcher)
    };
}
