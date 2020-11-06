/*
  ==============================================================================

    blueprint_ReactApplicationRoot.h
    Created: 9 Dec 2018 10:20:37am

  ==============================================================================
*/

#pragma once

#include <optional>

#include "blueprint_CanvasView.h"
#include "blueprint_EcmascriptEngine.h"
#include "blueprint_ImageView.h"
#include "blueprint_RawTextView.h"
#include "blueprint_ScrollView.h"
#include "blueprint_ScrollViewContentShadowView.h"
#include "blueprint_ShadowView.h"
#include "blueprint_TextShadowView.h"
#include "blueprint_TextView.h"
#include "blueprint_View.h"
#include "blueprint_ViewManager.h"


namespace blueprint
{
    //==============================================================================
    // Helper class which watches JS bundle files for changes and triggers
    // a user supplied callback in the event of a bundle file change.
    // BundleWatcher is able to handle multiple bundles to support loading
    // of multiple bundle files by EcmaScriptEngine in the future.
    class BundleWatcher : private juce::Timer
    {
        struct WatchedBundle
        {
            juce::File bundle;
            juce::Time bundleLastModifiedTime;
        };

    public:
        using BundleChangedCallback = std::function<void(const juce::File&)>;

        explicit BundleWatcher(BundleChangedCallback onBundleChangedCallback)
            : onBundleChanged(std::move(onBundleChangedCallback))
        {
            startTimer(50);
        }

        void watch(const juce::File& bundle)
        {
            watchedBundles.push_back({ bundle, bundle.getLastModificationTime() });
        }

        bool watching(const juce::File &bundle) const
        {
            return std::find_if(  watchedBundles.cbegin()
                                , watchedBundles.cend()
                                , [=](const WatchedBundle& wb)
                                  {
                                      return wb.bundle.getFullPathName() == bundle.getFullPathName();
                                  }) != watchedBundles.cend();
        }

    private:
        void timerCallback() override
        {
            std::for_each(watchedBundles.begin(), watchedBundles.end(), [=] (WatchedBundle& wb)
            {
                // Sanity check
                jassert(wb.bundle.existsAsFile());

                if (wb.bundle.existsAsFile())
                {
                    const auto lmt = wb.bundle.getLastModificationTime();

                    // In some instances webpack rebuilds temporarily result in an empty
                    // bundle file. We do not want to trigger evaluation callbacks when the
                    // bundle file is empty as this is likely to cause errors when users attempt
                    // to call javascript functions from C/C++. Calling eval/call in duktape with
                    // an empty js file/string does not result in an error and simply pushes undefined
                    // to the top of the duktape stack.
                    if (lmt > wb.bundleLastModifiedTime && wb.bundle.loadFileAsString().isNotEmpty())
                    {
                        onBundleChanged(wb.bundle);
                        wb.bundleLastModifiedTime = lmt;
                    }
                }
            });
        }

        std::vector<WatchedBundle> watchedBundles;
        BundleChangedCallback      onBundleChanged;
    };

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
        ReactApplicationRoot()
        {
            JUCE_ASSERT_MESSAGE_THREAD

            // Initialise the ViewManager
            initViewManager();

#if JUCE_DEBUG
            enableHotReloading();

            // Enable keyboardFocus to support CTRL-D/CMD-D debug attachment.
            setWantsKeyboardFocus(true);
#endif
        }

        //==============================================================================
        /** Override the default View behavior.  */
        void resized() override
        {
            // ViewManager may have been reset in the event of a bundle eval error.
            if (viewManager)
                viewManager->performRootShadowTreeLayout();
        }

        void paint(juce::Graphics& g) override
        {
            if (errorText)
            {
                g.fillAll(juce::Colour(0xffe14c37));
                errorText->draw(g, getLocalBounds().toFloat().reduced(10.f));
            }
            else
            {
                View::paint(g);
            }
        }
        //==============================================================================
        bool keyPressed(const juce::KeyPress& key) override
        {
            juce::ignoreUnused(key);
#if JUCE_DEBUG
           const auto startDebugCommand = juce::KeyPress('d', juce::ModifierKeys::commandModifier, 0);

           if (key == startDebugCommand)
           {
               engine.debuggerAttach();
           }
#endif
            return true;
        }

        //==============================================================================
        /**
         * Evaluates a javascript bundle file in the Ecmascript engine.
         *
         *  If hot-reload functionality has been enabled via enableHotReload(), ReactApplicationRoot will watch the
         *  supplied bundle file for changes and provide instant UI updates. This is useful when working with Webpack
         *  watchers etc.
         *
         *  Hot reloading is enabled by default in debug builds if JUCE_DEBUG is set.
         **/
        juce::var evaluate(const juce::File& bundle)
        {
            JUCE_ASSERT_MESSAGE_THREAD

            // Clear error state from previous js evals
            errorText.reset();

            if (hotReloadEnabled)
            {
                jassert(bundleWatcher);

                if (!bundleWatcher->watching(bundle))
                    bundleWatcher->watch(bundle);
            }

            try
            {
                // Register internal React.js backend rendering methods
                registerNativeRenderingHooks();

                if (beforeBundleEval)
                    beforeBundleEval(bundle);

                auto result = engine.evaluate(bundle);

                if (afterBundleEval)
                    afterBundleEval(bundle);

                return result;
            }
            catch (const EcmascriptEngine::Error& err)
            {
                handleRuntimeError(err);
                return juce::var();
            }
        }

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
                engine.invoke("__BlueprintNative__.dispatchEvent", eventType, std::forward<T>(args)...);
            } catch (const EcmascriptEngine::Error& err) {
                handleRuntimeError(err);
            }
        }

        //==============================================================================
        void registerViewType(const juce::String& typeId, ViewManager::ViewFactory f)
        {
            getViewManager().registerViewType(typeId, f);
        }

        ViewManager& getViewManager()
        {
            JUCE_ASSERT_MESSAGE_THREAD
            jassert (viewManager != nullptr);
            return *viewManager.get();
        }

        //==============================================================================
        /**
         * Enables "hot-reload" functionality so that ReactApplicationRoot watches JS bundle files added via evaluate()
         * for changes. When one of these bundle files changes (i.e. from a Webpack watch/rebuild) ReactApplicationRoot
         * will reload the bundle and update the View/ShadowView tree. Note hot-reloading is enabled by default if
         * JUCE_DEBUG is set.
         **/
        void enableHotReloading()
        {
            bundleWatcher = std::make_unique<BundleWatcher>(
                [=](const juce::File& bundle) {
                    handleBundleChanged(bundle);
                }
            );

            hotReloadEnabled = true;
        }

        //==============================================================================
        using BundleEvalCallback = std::function<void(const juce::File& bundle)>;

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

        //==============================================================================
        void handleRuntimeError(const EcmascriptEngine::Error& err)
        {
#if ! JUCE_DEBUG
            // In release builds, we don't catch errors and show the red screen,
            // we allow the exception to raise up to the user to be handled properly
            // for a production app.
            throw err;
#endif

            JUCE_ASSERT_MESSAGE_THREAD

            DBG("");
            DBG("==== Error in JavaScript runtime. Context: ====");
            DBG(err.context);
            DBG("");
            DBG(err.what());

            errorText = std::make_unique<juce::AttributedString>(err.stack);

#if JUCE_WINDOWS
            errorText->setFont(juce::Font("Lucida Console", 18, juce::Font::FontStyleFlags::plain));
#elif JUCE_MAC
            errorText->setFont(juce::Font("Monaco", 18, juce::Font::FontStyleFlags::plain));
#else
            errorText->setFont(18);
#endif

            // Lastly, kill the ViewManager to tear down existing views and prevent
            // further view interaction
            viewManager = nullptr;

            repaint();
        }

        //==============================================================================
        /** The ReactApplicationRoot's engine instance. */
        EcmascriptEngine engine;

    private:
        //==============================================================================
        void handleBundleChanged(const juce::File& bundle)
        {
            JUCE_ASSERT_MESSAGE_THREAD

            engine.reset();
            initViewManager();
            evaluate(bundle);
        }

        //==============================================================================
        /** Initialises the ViewManager and registers our native view types */
        void initViewManager()
        {
            viewManager = std::make_unique<ViewManager>(this);
            installNativeViewTypes();
        }

        //==============================================================================
        /** Registers each of the natively supported view types. */
        void installNativeViewTypes()
        {
            using ViewPair = ViewManager::ViewPair;

            jassert(viewManager);

            viewManager->registerViewType("Text", []() -> ViewPair {
                auto view = std::make_unique<TextView>();
                auto shadowView = std::make_unique<TextShadowView>(view.get());

                return {std::move(view), std::move(shadowView)};
            });

            viewManager->registerViewType("View", []() -> ViewPair {
                auto view = std::make_unique<View>();
                auto shadowView = std::make_unique<ShadowView>(view.get());

                return {std::move(view), std::move(shadowView)};
            });

            viewManager->registerViewType("CanvasView", []() -> ViewPair {
                auto view = std::make_unique<CanvasView>();
                auto shadowView = std::make_unique<ShadowView>(view.get());

                return {std::move(view), std::move(shadowView)};
            });

            viewManager->registerViewType("Image", []() -> ViewPair {
                auto view = std::make_unique<ImageView>();

                // ImageView does not need a specialized shadow view, unless
                // we want to enforce at the ShadowView level that it cannot
                // take children.
                auto shadowView = std::make_unique<ShadowView>(view.get());

                return {std::move(view), std::move(shadowView)};
            });

            viewManager->registerViewType("ScrollView", []() -> ViewPair {
                auto view = std::make_unique<ScrollView>();
                auto shadowView = std::make_unique<ShadowView>(view.get());

                return {std::move(view), std::move(shadowView)};
            });

            viewManager->registerViewType("ScrollViewContentView", []() -> ViewPair {
                auto view = std::make_unique<View>();
                auto shadowView = std::make_unique<ScrollViewContentShadowView>(view.get());

                return {std::move(view), std::move(shadowView)};
            });
        }

        void registerNativeRenderingHooks()
        {
            engine.registerNativeProperty("__BlueprintNative__", juce::JSON::parse("{}"));

            engine.registerNativeMethod("__BlueprintNative__", "createViewInstance", [this](const juce::var::NativeFunctionArgs& args) {
                jassert (args.numArguments == 1);

                auto viewType = args.arguments[0].toString();
                ViewId viewId = getViewManager().createViewInstance(viewType);

                return juce::var(viewId);
            });

            engine.registerNativeMethod("__BlueprintNative__", "createTextViewInstance", [this](const juce::var::NativeFunctionArgs& args) {
                jassert (args.numArguments == 1);

                auto textValue = args.arguments[0].toString();
                auto viewId = getViewManager().createTextViewInstance(textValue);

                return juce::var(viewId);
            });

            engine.registerNativeMethod("__BlueprintNative__", "setViewProperty", [this](const juce::var::NativeFunctionArgs& args) {
                jassert (args.numArguments == 3);
                jassert(viewManager);

                ViewId viewId = args.arguments[0];
                auto propertyName = args.arguments[1].toString();
                auto propertyValue = args.arguments[2];

                getViewManager().setViewProperty(viewId, propertyName, propertyValue);
                return juce::var::undefined();
            });

            engine.registerNativeMethod("__BlueprintNative__", "setRawTextValue", [this](const juce::var::NativeFunctionArgs& args) {
                jassert (args.numArguments == 2);
                jassert(viewManager);

                ViewId viewId = args.arguments[0];
                auto textValue = args.arguments[1].toString();

                getViewManager().setRawTextValue(viewId, textValue);
                return juce::var::undefined();
            });

            engine.registerNativeMethod("__BlueprintNative__", "addChild", [this](const juce::var::NativeFunctionArgs& args) {
                jassert (args.numArguments >= 2);
                jassert(viewManager);

                ViewId parentId = args.arguments[0];
                ViewId childId = args.arguments[1];
                int index = -1;

                if (args.numArguments > 2)
                    index = args.arguments[2];

                getViewManager().addChild(parentId, childId, index);
                return juce::var::undefined();
            });

            engine.registerNativeMethod("__BlueprintNative__", "removeChild", [this](const juce::var::NativeFunctionArgs& args) {
                jassert (args.numArguments == 2);
                jassert(viewManager);

                ViewId parentId = args.arguments[0];
                ViewId childId = args.arguments[1];

                getViewManager().removeChild(parentId, childId);
                return juce::var::undefined();
            });

            engine.registerNativeMethod("__BlueprintNative__", "getRootInstanceId", [this](const juce::var::NativeFunctionArgs& args) {
                jassert (args.numArguments == 0);
                juce::ignoreUnused(args);
                return juce::var(getViewId());
            });
        }

        //==============================================================================
        bool hotReloadEnabled = false;

        std::unique_ptr<ViewManager>            viewManager;
        std::unique_ptr<BundleWatcher>          bundleWatcher;
        std::unique_ptr<juce::AttributedString> errorText;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReactApplicationRoot)
    };
}
