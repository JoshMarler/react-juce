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

    private:
        //==============================================================================
        ViewManager viewManager;

        std::shared_ptr<EcmascriptEngine>       engine;
        std::unique_ptr<juce::AttributedString> errorText;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReactApplicationRoot)
    };
}
