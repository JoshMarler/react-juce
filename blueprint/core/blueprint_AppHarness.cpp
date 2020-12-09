namespace blueprint
{
    //==============================================================================
    AppHarness::AppHarness(ReactApplicationRoot& rar) :
        appRoot (rar)
    {
        JUCE_ASSERT_MESSAGE_THREAD

        fileWatcher = std::make_unique<FileWatcher> ([this]()
        {
            appRoot.reset();
            appRoot.bindNativeRenderingHooks();

            if (onBeforeAll != nullptr)
                onBeforeAll();

            for (const auto& f : fileWatcher->getWatchedFiles())
            {
                if (onBeforeEach != nullptr)
                    onBeforeEach (f);

                appRoot.evaluate (f);

                if (onAfterEach != nullptr)
                    onAfterEach (f);
            }

            if (onAfterAll != nullptr)
                onAfterAll();
        });
    }

    //==============================================================================
    void AppHarness::watch (const juce::File& f)
    {
        if (fileWatcher != nullptr)
            fileWatcher->watch (f);
    }

    void AppHarness::watch (const std::vector<juce::File>& fs)
    {
        JUCE_ASSERT_MESSAGE_THREAD

        if (fileWatcher != nullptr)
            for (const auto& f : fs)
                fileWatcher->watch (f);
    }

    void AppHarness::start()
    {
        if (onBeforeAll != nullptr)
            onBeforeAll();

        for (const auto& f : fileWatcher->getWatchedFiles())
        {
            if (onBeforeEach != nullptr)
                onBeforeEach (f);

            appRoot.evaluate (f);

            if (onAfterEach != nullptr)
                onAfterEach (f);
        }

        if (onAfterAll != nullptr)
            onAfterAll();

       #if JUCE_DEBUG
        if (fileWatcher != nullptr)
            fileWatcher->start();
       #endif
    }

    void AppHarness::stop()
    {
        if (fileWatcher != nullptr)
            fileWatcher->stop();
    }
}
