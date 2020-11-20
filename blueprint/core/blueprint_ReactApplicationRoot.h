/*
  ==============================================================================

    blueprint_ReactApplicationRoot.h
    Created: 9 Dec 2018 10:20:37am

  ==============================================================================
*/

#pragma once

#include <optional>

#include "blueprint_EcmascriptEngine.h"
#include "blueprint_FileWatcher.h"
#include "blueprint_View.h"
#include "blueprint_ViewManager.h"


namespace blueprint
{

    //==============================================================================
    /** The ReactApplicationRoot class prepares and maintains a Duktape evaluation
        context with the relevant hooks for supporting the Blueprint render
        backend.

        ReactApplicationRoot provides some useful facilities in debug builds with
        JUCE_DEBUG defined.

        Firstly, ReactApplicationRoot will automatically watch bundle
        files (added via ReactApplicationRoot::evaluate) for changes and reload said
        bundle in the event of a change/recompile. This allows for hot-reload functionality.

        Secondly, ReactApplicationRoot provides debug functionality similar to React Native.
        Users can hit CTRL-D/CMD-D when the ReactApplicationRoot component has focus,
        causing the application to suspend execution and await connection from a debug client.
        See the WIKI for details on setting up and connecting a JS debugger.

        Finally, ReactApplicationRoot implements a generic error handler which will catch
        errors from JavaScript code and display an error screen with an error trace/message.
        This generic error handler is enabled by default in both debug and release builds.
        Users of ReactApplicationRoot may which to override this handler by setting the
        EcmascriptEngine::onUncaughtError callback after constructing a ReactApplicationRoot instance.
        This can be useful to hide error details from users in production etc.
     */
    class ReactApplicationRoot : public View
    {
    public:
        //==============================================================================
        ReactApplicationRoot(std::shared_ptr<EcmascriptEngine> ee);
        ReactApplicationRoot();

        //==============================================================================
        /** Override the default resized behavior. */
        void resized() override;

        /** Override the default paint behavior. */
        void paint(juce::Graphics& g) override;

#if JUCE_DEBUG
        /** In debug builds, we add a keypress handler to toggle debugging. */
        bool keyPressed(const juce::KeyPress& key) override;
#endif

        //==============================================================================
        /** Evaluates a javascript bundle file in the Ecmascript engine.
         *
         * With the default behavior, ReactApplicationRoot will watch the supplied bundle
         * for changes and provide instant updates. If you're managing your own EcmascriptEngine,
         * you'll need to manage hot reloading on your own.
         */
        juce::var evaluate(const juce::File& bundle);

        /** Install a custom view type into the view manager. */
        void registerViewType(const juce::String& typeId, ViewManager::ViewFactory f);

        /** Dispatches an event through Blueprint's EventBridge. */
        template <typename... T>
        void dispatchEvent (const juce::String& eventType, T... args)
        {
            JUCE_ASSERT_MESSAGE_THREAD

            // We early return here in the event that we're currently showing the red error
            // screen. This prevents subsequent errors caused by dispatching events with an
            // incorrect engine state from overwriting the first error message.
            if (errorText)
                return;

            try {
                engine->invoke("__BlueprintNative__.dispatchEvent", eventType, std::forward<T>(args)...);
            } catch (const EcmascriptEngine::Error& err) {
                handleRuntimeError(err);
            }
        }

        /** Dispatches a view event through Blueprint's internal event replayer. */
        template <typename... T>
        void dispatchViewEvent (T... args)
        {
            JUCE_ASSERT_MESSAGE_THREAD

            // We early return here in the event that we're currently showing the red error
            // screen. This prevents subsequent errors caused by dispatching events with an
            // incorrect engine state from overwriting the first error message.
            if (errorText)
                return;

            try {
                engine->invoke("__BlueprintNative__.dispatchViewEvent", std::forward<T>(args)...);
            } catch (const EcmascriptEngine::Error& err) {
                handleRuntimeError(err);
            }
        }

        //==============================================================================
        /** Enables hot reloading (on by default in debug builds). */
        void enableHotReloading();

        /** Disables hot reloading (on by default in debug builds). */
        void disableHotReloading();

        //==============================================================================
        /** Displays the red error screen for the given error. */
        void handleRuntimeError(const EcmascriptEngine::Error& err);

        /** Clears the EcmascriptEngine and the view table. */
        void reset();

        /** Installs the rendering hooks needed by the React reconciler into the
         *  EcmascriptEngine environment.
         *
         *  Should be called after every `reset` and before evaluating the bundle.
         *  When using the default ReactApplicationRoot, this happens automatically.
         */
        void bindNativeRenderingHooks();

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
        ViewManager viewManager;

        std::shared_ptr<EcmascriptEngine>       engine;
        std::unique_ptr<FileWatcher>            fileWatcher;
        std::unique_ptr<juce::AttributedString> errorText;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReactApplicationRoot)
    };
}
