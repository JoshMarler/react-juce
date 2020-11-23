/*
  ==============================================================================

    blueprint_AppHarness.cpp
    Created: 21 Nov 2020 11:27:37am

  ==============================================================================
*/

#pragma once

#include "blueprint_AppHarness.h"


namespace blueprint
{

    //==============================================================================
    AppHarness::AppHarness(ReactApplicationRoot& appRoot)
    {
        JUCE_ASSERT_MESSAGE_THREAD

        fileWatcher = std::make_unique<FileWatcher>([this, &appRoot]() {
            appRoot.reset();
            appRoot.bindNativeRenderingHooks();

            if (onBeforeAll) { onBeforeAll(); }

            for (const auto& f : fileWatcher->getWatchedFiles())
            {
                if (onBeforeEach) { onBeforeEach(f); }
                appRoot.evaluate(f);
                if (onAfterEach) { onAfterEach(f); }
            }

            if (onAfterAll) { onAfterAll(); }
        });

#if JUCE_DEBUG
        fileWatcher->start();
#endif
    }

    //==============================================================================
    void AppHarness::watch (const juce::File& f)
    {
        if (fileWatcher)
            fileWatcher->watch(f);
    }

    void AppHarness::start()
    {
        if (fileWatcher)
            fileWatcher->start();
    }

    void AppHarness::stop()
    {
        if (fileWatcher)
            fileWatcher->stop();
    }

}
