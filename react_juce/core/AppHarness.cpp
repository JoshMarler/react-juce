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

        fileWatcher = std::make_unique<FileWatcher>([this]() {
            appRoot.reset();
            appRoot.bindNativeRenderingHooks();

            if (onBeforeAll) { onBeforeAll(); }

            for (const auto& f : fileWatcher->getWatchedFiles())
            {
                if (onBeforeEach) { onBeforeEach(f); }

                try
                {
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
        });

    }

    //==============================================================================
    void AppHarness::watch (const juce::File& f)
    {
        if (fileWatcher)
            fileWatcher->watch(f);
    }

    void AppHarness::watch (const std::vector<juce::File>& fs)
    {
        if (fileWatcher)
        {
            for (const auto& f : fs)
            {
                fileWatcher->watch(f);
            }
        }
    }

    void AppHarness::start()
    {
        // Nothing to do if we haven't watched any files
        if (fileWatcher == nullptr)
            return;

        if (onBeforeAll) { onBeforeAll(); }

        for (const auto& f : fileWatcher->getWatchedFiles())
        {
            if (onBeforeEach) { onBeforeEach(f); }

            try
            {
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

        if (onBeforeAll) { onBeforeAll(); }

        for (const auto& f : fileWatcher->getWatchedFiles())
        {
            if (onBeforeEach) { onBeforeEach(f); }
            appRoot.evaluate(f);
            if (onAfterEach) { onAfterEach(f); }
        }

        if (onAfterAll) { onAfterAll(); }
    }

}
