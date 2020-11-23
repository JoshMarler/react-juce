/*
  ==============================================================================

    blueprint_HotReloadingHarness.cpp
    Created: 21 Nov 2020 11:27:37am

  ==============================================================================
*/

#pragma once

#include "blueprint_HotReloadingHarness.h"


namespace blueprint
{

    //==============================================================================
    HotReloadingHarness::HotReloadingHarness(ReactApplicationRoot& appRoot)
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
    void HotReloadingHarness::watch (const juce::File& f)
    {
        if (fileWatcher)
            fileWatcher->watch(f);
    }

    void HotReloadingHarness::start()
    {
        if (fileWatcher)
            fileWatcher->start();
    }

    void HotReloadingHarness::stop()
    {
        if (fileWatcher)
            fileWatcher->stop();
    }

}
