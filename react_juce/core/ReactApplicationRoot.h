/*
  ==============================================================================

    ReactApplicationRoot.h
    Created: 9 Dec 2018 10:20:37am

  ==============================================================================
*/

#pragma once

#include "EcmascriptEngine.h"
#include "FileWatcher.h"
#include "View.h"
#include "ViewManager.h"


namespace reactjuce
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
        /** The main rendering interface. */
        juce::var createViewInstance (const juce::String& viewType);
        juce::var createTextViewInstance (const juce::String& textValue);
        juce::var setViewProperty (const ViewId viewId, const juce::String& name, const juce::var& value);
        juce::var setRawTextValue (const ViewId viewId, const juce::String& value);
        juce::var insertChild (const ViewId parentId, const ViewId childId, int index);
        juce::var removeChild (const ViewId parentId, const ViewId childId);
        juce::var getRootInstanceId();
        juce::var resetAfterCommit();

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

        /** Overload for evaluating precompiled bytecode file in engines which support this.
         *
         * Provides default error handling to display the red screen with error
         * message and stack trace.
         */
        juce::var evaluateBytecode(const juce::File &code);

        /** Install a custom view type into the view manager. */
        void registerViewType(const juce::String& typeId, ViewManager::ViewFactory f);

        /** Dispatches an event through the EventBridge. */
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
                engine->invoke("__NativeBindings__.dispatchEvent", eventType, std::forward<T>(args)...);
            } catch (const EcmascriptEngine::Error& err) {
                handleRuntimeError(err);
            }
        }

        /** Dispatches a view event through the internal event replayer. */
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
                engine->invoke("__NativeBindings__.dispatchViewEvent", std::forward<T>(args)...);
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
         */
        void bindNativeRenderingHooks();

        /** Get a handle to the internal threadpool. */
        juce::ThreadPool& getThreadPool();

    private:
        //==============================================================================
        template <int NumParams, typename MethodType>
        juce::var invokeFromNativeFunction (MethodType method, const juce::var::NativeFunctionArgs& args)
        {
            static_assert (NumParams <= 4);

            if (args.numArguments != NumParams)
                return juce::var::undefined();

            if constexpr (NumParams == 0)    return (this->*method)();
            if constexpr (NumParams == 1)    return (this->*method)(args.arguments[0]);
            if constexpr (NumParams == 2)    return (this->*method)(args.arguments[0], args.arguments[1]);
            if constexpr (NumParams == 3)    return (this->*method)(args.arguments[0], args.arguments[1], args.arguments[2]);
            if constexpr (NumParams == 4)    return (this->*method)(args.arguments[0], args.arguments[1], args.arguments[2], args.arguments[3]);

            return {};
        }

        template <int NumParams, typename MethodType>
        void addMethodBinding (const char* ns, const char* name, MethodType method) {
            engine->registerNativeMethod(
                ns,
                name,
                [this, method] (const juce::var::NativeFunctionArgs& args) -> juce::var {
                    return invokeFromNativeFunction<NumParams>(method, args);
                }
            );
        }

        //==============================================================================
        ViewManager viewManager;

        // This will be used by components to asynchronously download content from an web url.
        juce::ThreadPool threadPool;

        std::shared_ptr<EcmascriptEngine>       engine;
        std::unique_ptr<juce::AttributedString> errorText;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReactApplicationRoot)
    };
}
