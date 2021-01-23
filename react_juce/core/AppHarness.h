/*
  ==============================================================================

    AppHarness.h
    Created: 21 Nov 2020 11:27:37am

  ==============================================================================
*/

#pragma once

#include "EcmascriptEngine.h"
#include "FileWatcher.h"
#include "ReactApplicationRoot.h"


namespace reactjuce
{

    /** The AppHarness is a simple class which composes over your ReactApplicationRoot
     *  to provide file watching and hot reloading behavior for the bundle files your app evaluates.
     *
     *  To use, you should only need to make a AppHarness, set it to `watch()` any
     *  of your files, and then call `start()` (called automatically in Debug builds).
     *
     *  For custom behavior, you can attach callbacks at each of the four reloading stages:
     *    onBeforeAll – Invoked before evaluating any watched bundle
     *    onBeforeEach – Invoked before evaluating each watched bundle
     *    onAfterEach – Invoked after evaluating each watched bundle
     *    onAfterAll – Invoked after evaluating all watched bundles
     *    onEvalError – Invoked for an uncaught exception during bundle evaluation
     */
    class AppHarness
    {
    public:
        //==============================================================================
        AppHarness(ReactApplicationRoot& _appRoot);

        //==============================================================================
        void watch (const juce::File& f);
        void watch (const std::vector<juce::File>& fs);

        /** Run the initial evaluation step and then watch for file changes. */
        void start();

        /** Stop any running file watch. */
        void stop();

        /** Run the initial evaluation step exactly once.
         *
         *  It may be helpful to use AppHarness::start in DEBUG builds when you want
         *  hot reloading for development, then switch in RELEASE builds to using
         *  AppHarness::once to ignore file watching when you don't need it.
         */
        void once();

        //==============================================================================
        /**
         * Called once, after a file change is detected, before any watched bundle is evaluated.
         *
         * Often this callback will be used to install custom native methods and properties
         * that the code in the bundle will expect to find when it's evaluated.
         *
         * @code
         *
         *  MyEditor()
         *     : engine(std::make_shared<EcmascriptEngine>())
         *     , appRoot(engine)
         *     , harness(appRoot)
         * {
         *     juce::File myAppBundle("/path/to/myAppBundle.js");
         *
         *     harness.onBeforeAll = [=]() {
         *         engine->registerNativeMethod(
         *             "myNativeMethod",
         *             [this](const juce::var::NativeFunctionArgs& args) {
         *                 const juce::String& someParam = args.arguments[0].toString();
         *                 myMethod(someParam);
         *                 return juce::var::undefined();
         *             }
         *         );
         *     };
         *
         *     harness.watch(myAppBundle);
         *
         *     // Not strictly required if JUCE_DEBUG set.
         *     harness.start();
         * }
         *
         * @endcode
         **/
        std::function<void(void)> onBeforeAll;

        /**
         * Called after `onBeforeAll` immediately before evaluating each of the watched bundles.
         *
         * This callback may be used to assign specific behavior that must be invoked
         * before a watched bundle is evaluated, but in such a way that depends on
         * which bundle is being evaluated and when.
         *
         * @code
         *
         *  MyEditor()
         *     : engine(std::make_shared<EcmascriptEngine>())
         *     , appRoot(engine)
         *     , harness(appRoot)
         * {
         *     juce::File myAppBundle("/path/to/myAppBundle.js");
         *
         *     harness.onBeforeEach = [=](const juce::File& bundle) {
         *         if (bundle == myAppBundle) {
         *             engine->registerNativeMethod(
         *                 "myNativeMethod",
         *                 [this](const juce::var::NativeFunctionArgs& args) {
         *                     const juce::String& someParam = args.arguments[0].toString();
         *                     myMethod(someParam);
         *                     return juce::var::undefined();
         *                 }
         *             );
         *         }
         *     };
         *
         *     harness.watch(myAppBundle);
         *
         *     // Not strictly required if JUCE_DEBUG set.
         *     harness.start();
         * }
         *
         * @endcode
         **/
        std::function<void(const juce::File&)> onBeforeEach;

        /**
         * Called after `onBeforeEach` immediately after evaluating each of the watched bundles.
         *
         * This callback may be used to assign specific behavior that must be invoked
         * after a watched bundle is evaluated, but in such a way that depends on
         * which bundle hs been evaluated.
         *
         * @code
         *
         *  MyEditor()
         *     : engine(std::make_shared<EcmascriptEngine>())
         *     , appRoot(engine)
         *     , harness(appRoot)
         * {
         *     juce::File myAppBundle("/path/to/myAppBundle.js");
         *
         *     harness.onAfterEach = [=](const juce::File& bundle) {
         *         if (bundle == myAppBundle) {
         *             engine->invoke("MyBundlePlacedThisHere", 42);
         *         }
         *     };
         *
         *     harness.watch(myAppBundle);
         *
         *     // Not strictly required if JUCE_DEBUG set.
         *     harness.start();
         * }
         *
         * @endcode
         **/
        std::function<void(const juce::File&)> onAfterEach;

        /**
         * Called once, after all the watched files have been evaluated and all the
         * prior callbacks have been invoked.
         *
         * Often this callback will be used to dispatch events or values into an
         * application that is expecting some initial state.
         *
         * @code
         *
         *  MyEditor()
         *     : engine(std::make_shared<EcmascriptEngine>())
         *     , appRoot(engine)
         *     , harness(appRoot)
         * {
         *     juce::File myAppBundle("/path/to/myAppBundle.js");
         *
         *     harness.onAfterAll = [=]() {
         *         appRoot.dispatchEvent("InitialParameterState, getInitialParamState());
         *     };
         *
         *     harness.watch(myAppBundle);
         *
         *     // Not strictly required if JUCE_DEBUG set.
         *     harness.start();
         * }
         *
         * @endcode
         **/
        std::function<void(void)> onAfterAll;

        /**
         * Called for any uncaught exception during bundle evaluation.
         *
         * @code
         *
         *  MyEditor()
         *     : engine(std::make_shared<EcmascriptEngine>())
         *     , appRoot(engine)
         *     , harness(appRoot)
         * {
         *     juce::File myAppBundle("/path/to/myAppBundle.js");
         *
         *     harness.onEvalError = [=](const EcmascriptEngine::Error& e) {
         *         logError(e.what());
         *     };
         *
         *     harness.watch(myAppBundle);
         *     harness.start();
         * }
         *
         * @endcode
         **/
        std::function<void(const EcmascriptEngine::Error&)> onEvalError;

    private:
        //==============================================================================
        ReactApplicationRoot& appRoot;
        std::unique_ptr<FileWatcher> fileWatcher;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AppHarness)
    };

}
