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
    HotReloadingHarness::HotReloadingHarness(ReactApplicationRoot& appRoot, std::shared_ptr<EcmascriptEngine> ee)
    {
        JUCE_ASSERT_MESSAGE_THREAD

        fileWatcher = std::make_unique<FileWatcher>([this, &appRoot, ee]() {
            appRoot.reset();
            appRoot.bindNativeRenderingHooks();

            // TODO: I could imagine support 4 hooks here: beforeAll, beforeEach, afterEach, beforeAll,
            // where the middle two callbacks actually get specific references to _which_ bundle
            if (beforeBundleEval)
                beforeBundleEval(ee, juce::File());

            for (auto& f : fileWatcher->getWatchedFiles())
                ee->evaluate(f);

            if (afterBundleEval)
                afterBundleEval(ee, juce::File());
        });

#if JUCE_DEBUG
        fileWatcher->start();
#endif
    }

    HotReloadingHarness::HotReloadingHarness(ReactApplicationRoot& appRoot)
    {
        JUCE_ASSERT_MESSAGE_THREAD

        fileWatcher = std::make_unique<FileWatcher>([this, &appRoot]() {
            appRoot.reset();
            appRoot.bindNativeRenderingHooks();

            if (beforeBundleEval)
                beforeBundleEval(nullptr, juce::File());

            for (auto& f : fileWatcher->getWatchedFiles())
                appRoot.evaluate(f);

            if (afterBundleEval)
                afterBundleEval(nullptr, juce::File());
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
