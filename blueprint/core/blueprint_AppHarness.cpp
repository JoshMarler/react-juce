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
                appRoot.evaluate(f);
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
        if (onBeforeAll) { onBeforeAll(); }

        for (const auto& f : fileWatcher->getWatchedFiles())
        {
            if (onBeforeEach) { onBeforeEach(f); }
            appRoot.evaluate(f);
            if (onAfterEach) { onAfterEach(f); }
        }

        if (onAfterAll) { onAfterAll(); }

#if JUCE_DEBUG
        if (fileWatcher)
            fileWatcher->start();
#endif
    }

    void AppHarness::stop()
    {
        if (fileWatcher)
            fileWatcher->stop();
    }

}
