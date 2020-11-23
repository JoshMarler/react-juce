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
    /** The ReactApplicationRoot class is the primary point of coordination between
     *  the React.js reconciler and the native View heirarchy.
     *
     *  By default, ReactApplicationRoot implements a generic error handler which will
     *  catch errors from JavaScript code and display an error screen with a stack trace
     *  and error message. This generic error handler is enabled by default in Debug builds,
     *  and in Release builds such errors will be thrown, with the intention that the end
     *  user should catch and handle them appropriately.
     *
     *  ReactApplicationRoot also provides debug functionality similar to React Native.
     *  Users can hit CTRL-D/CMD-D when the ReactApplicationRoot component has focus,
     *  causing the application to suspend execution and await connection from a debug client.
     *  See the documentation for details on setting up and connecting a debugger.
     */
    class ReactApplicationRoot : public View
    {
    public:
        //==============================================================================
        explicit ReactApplicationRoot(std::shared_ptr<EcmascriptEngine> ee);
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
        /** Evaluates a javascript bundle file in the EcmascriptEngine.
         *
         * Provides default error handling to display the red screen with error
         * message and stack trace.
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

        /** Clears the internal EcmascriptEngine and view table. */
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
