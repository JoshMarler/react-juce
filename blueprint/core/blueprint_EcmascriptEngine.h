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
        /** Evaluates the given code in the interpreter, ignoring the result but indicating
         *  success or failure via the returned Result.
         */
        juce::Result execute (const juce::String& code);

        /** Evaluates the given code in the interpreter, returning the result. */
        juce::var evaluate (const juce::String& code);

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

    private:
        //==============================================================================
        duk_context* ctx;

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
