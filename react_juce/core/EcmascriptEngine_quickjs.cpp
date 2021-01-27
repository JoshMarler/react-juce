/*
  ==============================================================================

    EcmascriptEngine_quickjs.cpp
    Created: 26 Jan 2021 8:08:39am

  ==============================================================================
*/

#include "EcmascriptEngine.h"

#include <quickjs/quickjs.h>
#include <quickjs/quickjs-libc.h>


namespace reactjuce
{

    namespace
    {

        static JSValue js_setTimeout(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);
        static JSValue js_clearTimeout(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);

        static const JSCFunctionListEntry platform_funcs[] = {
            JS_CFUNC_DEF("setTimeout", 2, js_setTimeout),
            JS_CFUNC_DEF("clearTimeout", 1, js_clearTimeout),
            JS_CFUNC_DEF("setInterval", 2, js_setTimeout),
            JS_CFUNC_DEF("clearInterval", 1, js_clearTimeout),
        };

    }

    //==============================================================================
    struct EcmascriptEngine::Pimpl
    {
        Pimpl()
        {
            rt = JS_NewRuntime();
            ctx = JS_NewContext(rt);

            // Add some default helpers, e.g. console.log
            js_std_add_helpers(ctx, 0, nullptr);

            // Add a pointer back to our pimpl instance
            JS_SetContextOpaque(ctx, (void *) this);

            // Register global helpers
            auto global = JS_GetGlobalObject(ctx);
            JS_SetPropertyFunctionList(ctx, global, platform_funcs, 4u);
        }

        ~Pimpl() = default;

        //==============================================================================
        juce::var evaluate (const juce::File& code)
        {
            auto name = code.getFileName();
            auto body = code.loadFileAsString();

            std::vector<char> rawBody (body.getNumBytesAsUTF8() + 1);
            strcpy(rawBody.data(), body.toRawUTF8());
            rawBody[body.getNumBytesAsUTF8()] = '\0';

            auto val = JS_Eval(ctx, rawBody.data(), rawBody.size(), name.toRawUTF8(), JS_EVAL_TYPE_GLOBAL);

            if (JS_IsException(val))
            {
                auto ex = JS_GetException(ctx);
                auto msg = JS_GetPropertyStr(ctx, ex, "message");
                auto stack = JS_GetPropertyStr(ctx, ex, "stack");

                throw EcmascriptEngine::Error(JS_ToCString(ctx, msg), JS_ToCString(ctx, stack));
            }

            return juce::var();
        }

        //==============================================================================
        JSValue setTimeout()
        {
            return JS_NewFloat64(ctx, 1);
        }

        JSValue clearTimeout()
        {
            return JS_NewFloat64(ctx, 1);
        }

        //==============================================================================
        // TODO: unique_ptrs with custom deallocators
        JSRuntime* rt;
        JSContext* ctx;
    };

    //==============================================================================
    // Platform function implementations
    namespace
    {

        JSValue js_setTimeout(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
        {
            if (auto* pimpl = static_cast<EcmascriptEngine::Pimpl*>(JS_GetContextOpaque(ctx)))
            {
                // TODO: Do this right
                return pimpl->setTimeout();
            }

            return JS_EXCEPTION;
        }

        JSValue js_clearTimeout(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
        {
            if (auto* pimpl = static_cast<EcmascriptEngine::Pimpl*>(JS_GetContextOpaque(ctx)))
            {
                // TODO: Do this right
                return pimpl->clearTimeout();
            }

            return JS_EXCEPTION;
        }
    }

    //==============================================================================
    EcmascriptEngine::EcmascriptEngine()
        : mPimpl(std::make_unique<Pimpl>())
    {
        /** If you hit this, you're probably trying to run a console application.

            Please make use of juce::ScopedJuceInitialiser_GUI because this JS engine requires event loops.
            Without the initialiser, the console app would always crash on exit,
            and things will probably not get cleaned up.
        */
        jassert (juce::MessageManager::getInstanceWithoutCreating() != nullptr);
    }

    EcmascriptEngine::~EcmascriptEngine() = default;

    //==============================================================================
    juce::var EcmascriptEngine::evaluateInline (const juce::String& code)
    {
        return juce::var("Not Implemented.");
    }

    juce::var EcmascriptEngine::evaluate (const juce::File& code)
    {
        return mPimpl->evaluate(code);
    }

    //==============================================================================
    void EcmascriptEngine::registerNativeMethod (const juce::String& name, juce::var::NativeFunction fn)
    {
        registerNativeProperty(name, juce::var(fn));
    }

    void EcmascriptEngine::registerNativeMethod (const juce::String& target, const juce::String& name, juce::var::NativeFunction fn)
    {
        registerNativeProperty(target, name, juce::var(fn));
    }

    //==============================================================================
    void EcmascriptEngine::registerNativeProperty (const juce::String& name, const juce::var& value)
    {
        juce::ignoreUnused(name);
        juce::ignoreUnused(value);
    }

    void EcmascriptEngine::registerNativeProperty (const juce::String& target, const juce::String& name, const juce::var& value)
    {
        juce::ignoreUnused(name);
        juce::ignoreUnused(value);
    }

    //==============================================================================
    juce::var EcmascriptEngine::invoke (const juce::String& name, const std::vector<juce::var>& vargs)
    {
        return juce::var("Not Implemented");
    }

    void EcmascriptEngine::reset() {}

    //==============================================================================
    void EcmascriptEngine::debuggerAttach() {}
    void EcmascriptEngine::debuggerDetach() {}

}
