/*
  ==============================================================================

    blueprint_HotReloadingHarness.h
    Created: 21 Nov 2020 11:27:37am

  ==============================================================================
*/

#pragma once

#include "blueprint_EcmascriptEngine.h"
#include "blueprint_FileWatcher.h"
#include "blueprint_ReactApplicationRoot.h"


namespace blueprint
{

    class HotReloadingHarness
    {
    public:
        //==============================================================================
        HotReloadingHarness(ReactApplicationRoot& appRoot, std::shared_ptr<EcmascriptEngine> ee);
        HotReloadingHarness(ReactApplicationRoot& appRoot);

        //==============================================================================
        void watch (const juce::File& f);
        void start();
        void stop();

        //==============================================================================
        using BundleEvalCallback = std::function<void(std::shared_ptr<EcmascriptEngine> e, const juce::File& bundle)>;

        /**
         * Called before a bundle is loaded/evaluated.
         *
         * If hot-reload functionality is enabled then this callback will be triggered whenever a previously
         * registered bundle file is changed (i.e. due to a Webpack watcher/rebuild).
         *
         * Use this callback to register any native methods/properties to be used from within the JS bundle. You can
         * also use this callback to register any custom error handlers via ReactApplicationRoot::onUncaughtError.
         *
         * @param bundle The reloaded bundle file.
         *
         * For most applications the bundle will simply be the main app root bundle file. However, other use cases
         * for evaluating multiple bundle files may exist, for example to facilitate things like polyfills etc.
         * As a result it is good practice for callback implementations to check that bundle is the expected file.
         *
         * @code
         *
         *  MyEditor()
         *     : appRoot()
         * {
         *     juce::File myAppBundle("/path/to/myAppBundle.js");
         *
         *     // Not strictly required if JUCE_DEBUG set.
         *     appRoot.enableHotReload(true);
         *
         *     appRoot.beforeBundleEval = [=](juce::File bundle)
         *     {
         *         if (bundle.getFullPathName() == myAppBundle.getFullPathName())
         *         {
         *              appRoot.registerNativeMethod(
         *                  "myNativeMethod",
         *                  [](void* stash, const juce::var::NativeFunctionArgs& args) {
         *                      auto* self = reinterpret_cast<MyEditor*>(stash);
         *
         *                      const juce::String& someParam = args.arguments[0].toString();
         *                      self->myNativeMethod(someParam);
         *
         *                      return juce::var::undefined();
         *                  },
         *                  (void*) this
         *              );
         *         }
         *         else
         *         {
         *             // You have loaded some other js bundle. i.e. a polyfill
         *         }
         *     };
         *
         *     appRoot.evaluate(myAppBundle);
         * }
         *
         * @endcode
         **/
        BundleEvalCallback beforeBundleEval;

        /**
         * Called after a bundle is loaded/evaluated.
         *
         * If hot-reload functionality is enabled then this callback will be triggered whenever a previously
         * registered bundle file is changed (i.e. due to a Webpack watcher/rebuild).
         *
         * Use this callback to dispatch any initial state/events required by the React/JS application on load/reload.
         *
         * @param bundle The reloaded bundle file.
         *
         * For most applications the bundle will simply be the main app root bundle file. However, other use cases
         * for evaluating multiple bundle files may exist, for example to facilitate things like polyfills etc.
         * As a result it is good practice for callback implementations to check that bundle is the expected file.
         *
         * @code
         *
         *  MyEditor()
         *     : appRoot()
         * {
         *     juce::File myAppBundle("/path/to/myAppBundle.js");
         *
         *     // Not strictly required if JUCE_DEBUG set.
         *     appRoot.enableHotReload(true);
         *
         *     appRoot.afterBundleEval = [=](const juce::File& bundle)
         *     {
         *         if (bundle.getFullPathName() == myAppBundle.getFullPathName())
         *         {
         *             juce::String message("This is an important message");
         *             appRoot.dispatchEvent("importantMessage", message);
         *         }
         *         else
         *         {
         *             // You have loaded some other js bundle. i.e. a polyfill
         *         }
         *     };
         *
         *     appRoot.evaluate(myAppBundle);
         * }
         *
         * @endcode
         **/
        BundleEvalCallback afterBundleEval;

    private:
        //==============================================================================
        std::unique_ptr<FileWatcher> fileWatcher;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HotReloadingHarness)
    };

}
