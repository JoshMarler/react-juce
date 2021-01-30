/*
  ==============================================================================

    AppHarness.cpp
    Created: 21 Nov 2020 11:27:37am

  ==============================================================================
*/

#include "AppHarness.h"


namespace reactjuce
{

    //==============================================================================
    AppHarness::AppHarness(ReactApplicationRoot& _appRoot)
        : appRoot(_appRoot)
    {
        JUCE_ASSERT_MESSAGE_THREAD
        fileWatcher = std::make_unique<FileWatcher>([this]() { handleFilesChanged(); });
    }

    //==============================================================================
    void AppHarness::watch (const juce::File& f)
    {
        if (fileWatcher)
        {
            sourceFileTypeMap[f.getFullPathName()] = false;
            fileWatcher->watch(f);
        }
    }

    void AppHarness::watch (const std::vector<juce::File>& fs)
    {
        if (fileWatcher)
        {
            for (const auto& f : fs)
                watch(f);
        }
    }

    void AppHarness::watchBytecode (const juce::File& f)
    {
        if (fileWatcher)
        {
            sourceFileTypeMap[f.getFullPathName()] = true;
            fileWatcher->watch(f);
        }
    }

    void AppHarness::watchBytecode (const std::vector<juce::File>& fs)
    {
        if (fileWatcher)
        {
            for (const auto& f : fs)
                watchBytecode(f);
        }
    }

    void AppHarness::start()
    {
        // Nothing to do if we haven't watched any files
        if (fileWatcher == nullptr)
            return;

        // Run initial callbacks and file eval.
        handleFilesChanged();

        // Finally, kick off the file watch process
        fileWatcher->start();
    }

    void AppHarness::stop()
    {
        if (fileWatcher)
            fileWatcher->stop();
    }

    void AppHarness::once()
    {
        // Nothing to do if we haven't watched any files
        if (fileWatcher == nullptr)
            return;

        // Run callback and eval step once.
        handleFilesChanged();
    }

    void AppHarness::handleFilesChanged()
    {
        appRoot.reset();
        appRoot.bindNativeRenderingHooks();

        if (onBeforeAll) { onBeforeAll(); }

        for (const auto& f : fileWatcher->getWatchedFiles())
        {
            if (onBeforeEach) { onBeforeEach(f); }

            try
            {
                const bool isBytecode = sourceFileTypeMap[f.getFullPathName()];

                if (isBytecode)
                    appRoot.evaluateBytecode(f);
                else
                    appRoot.evaluate(f);
            }
            catch (const EcmascriptEngine::Error& err)
            {
                // We may fall in here in Release builds when ReactApplicationRoot hasn't
                // caught the error for us to show the red screen. In this case, we call
                // a user supplied error handler if we have one, else just break the loop
                // and await the next file change event.
                if (onEvalError)
                {
                    onEvalError(err);
                    break;
                }
                else
                {
                    throw err;
                }
            }

            if (onAfterEach) { onAfterEach(f); }
        }

        if (onAfterAll) { onAfterAll(); }
    }

}
