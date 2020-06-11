/*
  ==============================================================================

    blueprint_EcmascriptEngine.h
    Created: 24 Oct 2019 3:08:39pm

  ==============================================================================
*/

#pragma once


namespace blueprint
{

    //==============================================================================
    /** The EcmascriptEngine provides a flexible ECMAScript 5 compliant JavaScript engine
     *  with an interface implemented by Duktape, but which may be implemented by one of
     *  many embedded engines in the future.
     */
    class EcmascriptEngine
    {
    public:
        //==============================================================================
        // Our notion of a NativeFunction must be compatible with Duktape's C API
        using NativeFunction = juce::var (*)(void *, const juce::var::NativeFunctionArgs&);

        //==============================================================================
        EcmascriptEngine();
        ~EcmascriptEngine();

        //==============================================================================
        /** Evaluates the given code in the interpreter, returning the result. */
        juce::var evaluate (const juce::String& code);
        juce::var evaluate (const juce::File& code);

        //==============================================================================
        /** Registers a native method by the given name in the global namespace. */
        void registerNativeMethod (const juce::String&, NativeFunction, void* = nullptr);

        /** Registers a native method by the given name on the target object.
         *
         *  The provided target name may be any expression that leaves the target
         *  object on the top of the stack. For example:
         *
         *  ```
         *  registerNativeMethod("global", "hello", []() {
         *      std::cout << "World!" << std::endl;
         *      return juce::var();
         *  });
         *  ```
         *
         *  is equivalent to calling the previous `registerNativeMethod` overload
         *  with just the "hello" and function arguments.
         */
        void registerNativeMethod (const juce::String&, const juce::String&, NativeFunction, void* = nullptr);

        //==============================================================================
        /** Registers a native value by the given name in the global namespace. */
        void registerNativeProperty (const juce::String&, const juce::var&);

        /** Registers a native value by the given name on the target object.
         *
         *  The provided target name may be any expression that leaves the target
         *  object on the top of the stack. For example, the following three
         *  examples have equivalent behavior:
         *
         *  ```
         *  registerNativeProperty("global", "hello", "world");
         *  registerNativeProperty("hello", "world");
         *  evaluate("global.hello = \"world\";");
         *  ```
         */
        void registerNativeProperty (const juce::String&, const juce::String&, const juce::var&);

        //==============================================================================
        /** Invokes a method, applying the given args, inside the interpreter.
         *
         *  This is similar in function to `Function.prototype.apply()`. The provided
         *  method name may be any expression that leaves the target function on the
         *  top of the stack. For example:
         *
         *  `invoke("BlueprintNative.dispatchViewEvent", args);`
         *
         *  Returns the result of the invocation.
         */
        juce::var invoke (const juce::String& name, const std::vector<juce::var>& vargs);

        /** Invokes a method with the given args inside the interpreter.
         *
         *  This is similar in function to `Function.prototype.call()`. The provided
         *  method name may be any expression that leaves the target function on the
         *  top of the stack. For example:
         *
         *  `invoke("BlueprintNative.dispatchViewEvent", "click");`
         *
         *  Returns the result of the invocation.
         */
        template <typename... T>
        juce::var invoke (const juce::String& name, T... args);

        //==============================================================================
        /** A public member which can be assigned a callback for delegating error handling to user code.
         *
         * This callback will be called primarily in the case of recoverable errors when evaluating JS code.
         * It is possible to continue using this EcmascriptEngine instance after such an error has been
         * raised. For example callers may wish to handle such an error and subsequently reload a modified
         * version of a JS bundle file as part of a "hot-reload" workflow, incrementally fixing/debugging
         * errors.
         */
        std::function<void(const juce::String& msg, const juce::String& trace)> onUncaughtError;

        //==============================================================================
        /**
         * Pauses execution and waits for a debug client to attach and begin a debug session.
         */
        void debuggerAttach();

        /**
         * Detaches the from the current debug session/attachment.
         */
        void debuggerDetach();

        //==============================================================================
        // TODO: These pushVarToDukStack/readVarFromDukStack should be private, but are
        // made public temporarily because I hacked together a nativeMethodWrapper hook
        // that needs it. Once we replace `nativeMethodWrapper` with the LambdaHelper
        // stuff below, these should be made private again.
        void pushVarToDukStack (duk_context* ctx, const juce::var& v);
        juce::var readVarFromDukStack (duk_context* ctx, duk_idx_t idx);

    private:
        //==============================================================================
        struct LambdaHelper {
            LambdaHelper(juce::var::NativeFunction fn);

            static duk_ret_t invokeFromDukContext(duk_context* ctx);
            static duk_ret_t callbackFinalizer (duk_context* ctx);

            juce::var::NativeFunction callback;
        };

        //==============================================================================
        duk_context* ctx;
        std::vector<std::unique_ptr<LambdaHelper>> lambdaReleasePool;

        //==============================================================================
        void removeLambdaHelper (LambdaHelper* helper);

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EcmascriptEngine)
    };

    //==============================================================================
    template <typename... T>
    juce::var EcmascriptEngine::invoke (const juce::String& name, T... args)
    {
        // Pack the args and push them to the alternate `invoke` implementation
        std::vector<juce::var> vargs { args... };
        return invoke(name, vargs);
    }

}
