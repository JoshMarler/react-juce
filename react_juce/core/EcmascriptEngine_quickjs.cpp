/*
  ==============================================================================

    EcmascriptEngine_quickjs.cpp
    Created: 26 Jan 2021 8:08:39am

  ==============================================================================
*/

#include "EcmascriptEngine.h"

#include <unordered_map>
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

        struct FuncTimer : public juce::Timer
        {
            using CallbackType = std::function<void(void)>;

            FuncTimer(CallbackType&& cb, double interval)
                : callback(std::move(cb))
            {
                timerId = nextTimerId++;
                startTimer(interval);
            }

            ~FuncTimer()
            {
                stopTimer();
            }

            void timerCallback() override
            {
                std::invoke(callback);
            }

            CallbackType callback;
            int32_t timerId;

            static int32_t nextTimerId;
        };

        int32_t FuncTimer::nextTimerId = 0;

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
        void registerNativeProperty (const juce::String& name, const juce::var& value)
        {
            auto global = JS_GetGlobalObject(ctx);
            JS_SetPropertyStr(ctx, global, name.toRawUTF8(), varToJSValue(ctx, value));
        }

        void registerNativeProperty (const juce::String& target, const juce::String& name, const juce::var& value)
        {
            auto val = JS_Eval(ctx, target.toRawUTF8(), target.getNumBytesAsUTF8(), "<eval>", JS_EVAL_TYPE_GLOBAL);

            if (JS_IsException(val))
            {
                auto ex = JS_GetException(ctx);
                auto msg = JS_GetPropertyStr(ctx, ex, "message");
                auto stack = JS_GetPropertyStr(ctx, ex, "stack");

                throw EcmascriptEngine::Error(JS_ToCString(ctx, msg), JS_ToCString(ctx, stack));
            }

            JS_SetPropertyStr(ctx, val, name.toRawUTF8(), varToJSValue(ctx, value));
        }

        //==============================================================================
        juce::var invoke(const juce::String& name, const std::vector<juce::var>& vargs)
        {
            auto funcVal = JS_Eval(ctx, name.toRawUTF8(), name.getNumBytesAsUTF8(), "<input>", JS_EVAL_TYPE_GLOBAL);

            if (JS_IsException(funcVal))
            {
                auto ex = JS_GetException(ctx);
                auto msg = JS_GetPropertyStr(ctx, ex, "message");
                auto stack = JS_GetPropertyStr(ctx, ex, "stack");

                throw EcmascriptEngine::Error(JS_ToCString(ctx, msg), JS_ToCString(ctx, stack));
            }

            if (JS_IsFunction(ctx, funcVal))
            {
                std::vector<JSValue> jsArgs;

                for (size_t i = 0; i < vargs.size(); ++i)
                    jsArgs.push_back(varToJSValue(ctx, vargs[i]));

                auto thisVal = JS_NULL;
                auto ret = JS_Call(ctx, funcVal, thisVal, (int) jsArgs.size(), jsArgs.data());

                // JS_Free ret? thisVal?
                return JSValueToVar(ctx, ret);
            }

            throw EcmascriptEngine::Error("Trying to invoke a non-function.", "No stack to show.");
        }

        //==============================================================================
        JSValue setTimeout(JSContext* ctx, JSValueConst thisVal, int argc, JSValueConst* argv)
        {
            if (argc < 1)
                return JS_EXCEPTION;

            std::vector<juce::var> args;

            for (int i = 0; i < argc; ++i)
                args.push_back(JSValueToVar(ctx, argv[i]));

            double const interval = (double) args[1];

            auto timer = std::make_unique<FuncTimer>([this, ctx_ = ctx, args = std::move(args)]() {
                juce::var::NativeFunctionArgs jsArgs(juce::var::undefined(), args.data() + 2, static_cast<int>(args.size() - 2));
                std::invoke(args[0].getNativeFunction(), jsArgs);
            }, interval);

            auto timerId = timer->timerId;
            funcTimers.push_back(std::move(timer));

            return JS_NewInt32(ctx, timerId);
        }

        JSValue clearTimeout()
        {
            return JS_NewFloat64(ctx, 1);
        }

        //==============================================================================
        JSValue varToJSValue (JSContext* ctx, const juce::var& v)
        {
            if (v.isVoid() || v.isUndefined())
                return JS_UNDEFINED;
            if (v.isBool() && (bool) v)
                return JS_TRUE;
            if (v.isBool() && !v)
                return JS_FALSE;
            if (v.isInt())
                return JS_NewInt32(ctx, static_cast<juce::int32>(v));
            if (v.isInt64())
                return JS_NewInt64(ctx, static_cast<juce::int64>(v));
            if (v.isDouble())
                return JS_NewFloat64(ctx, static_cast<double>(v));
            if (v.isString())
                return JS_NewString(ctx, v.toString().toRawUTF8());
            if (v.isArray())
            {
                auto ret = JS_NewArray(ctx);
                int32_t i = 0;

                for (auto& e : *(v.getArray()))
                {
                    JS_SetPropertyUint32(ctx, ret, i++, varToJSValue(ctx, e));
                }

                return ret;
            }
            if (v.isObject())
            {
                if (auto* o = v.getDynamicObject())
                {
                    auto ret = JS_NewObject(ctx);

                    for (auto& e : o->getProperties())
                    {
                        JS_SetPropertyStr(ctx, ret, e.name.toString().toRawUTF8(), varToJSValue(ctx, e.value));
                    }

                    return ret;
                }

                return JS_EXCEPTION;
            }
            if (v.isMethod())
            {
                auto magic = nextMagicInt++;
                lambdaReleasePool[magic] = std::move(v.getNativeFunction());

                return JS_NewCFunctionMagic(ctx, [](JSContext* ctx_, JSValueConst this_val, int argc, JSValueConst* argv, int magic) -> JSValue {
                    if (auto* pimpl = static_cast<EcmascriptEngine::Pimpl*>(JS_GetContextOpaque(ctx_)))
                    {
                        std::vector<juce::var> args;

                        for (int i = 0; i < argc; ++i)
                        {
                            args.push_back(pimpl->JSValueToVar(ctx_, argv[i]));
                        }

                        juce::var::NativeFunctionArgs jsArgs(juce::var::undefined(), args.data(), static_cast<int>(args.size()));
                        auto retVar = std::invoke(pimpl->lambdaReleasePool[magic], jsArgs);

                        return pimpl->varToJSValue(ctx_, retVar);
                    }

                    jassertfalse;
                    return JS_NewInt32(ctx_, 0);
                }, "<NativeFunction>", 0, JS_CFUNC_generic_magic, magic);
            }

            return JS_EXCEPTION;
        }

        juce::var JSValueToVar (JSContext* ctx, JSValueConst val)
        {
            if (JS_IsNull(val) || JS_IsUndefined(val))
                return juce::var::undefined();
            if (JS_IsBool(val))
                return juce::var(JS_ToBool(ctx, val));
            if (JS_IsNumber(val))
            {
                // How to check if int or double?
                // double pres;
                // JS_ToFloat64(ctx, &pres, val);
                // return juce::var(pres);
                int32_t pres;
                double fpres;
                // if (JS_ToInt32(ctx, &pres, val) >= 0)
                //     return juce::var(pres);
                if (JS_ToFloat64(ctx, &fpres, val) >= 0)
                    return juce::var(fpres);

                jassertfalse;
                return juce::var();
            }
            if (JS_IsString(val))
            {
                auto strVal = juce::String(juce::CharPointer_UTF8(JS_ToCString(ctx, val)));
                return juce::var(strVal);
            }
            if (JS_IsFunction(ctx, val))
            {
                return juce::var::NativeFunction {
                    [this, ctx_ = ctx, funcVal = val](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        std::vector<JSValue> jsArgs;

                        for (int i = 0; i < args.numArguments; ++i)
                            jsArgs.push_back(varToJSValue(ctx_, args.arguments[i]));

                        auto thisVal = JS_NULL;
                        auto ret = JS_Call(ctx_, funcVal, thisVal, (int) jsArgs.size(), jsArgs.data());

                        // JS_Free ret? thisVal?
                        return JSValueToVar(ctx_, ret);
                    }
                };
            }
            if (JS_IsArray(ctx, val))
            {
                juce::Array<juce::var> els;
                int32_t length;

                // Read the JSValue array length
                JS_ToInt32(ctx, &length, JS_GetPropertyStr(ctx, val, "length"));

                for (int32_t i = 0; i < length; ++i)
                {
                    auto arrVal = JS_GetPropertyUint32(ctx, val, static_cast<uint32_t>(i));
                    els.add(JSValueToVar(ctx, arrVal));
                    JS_FreeValue(ctx, arrVal);
                }

                return juce::var(els);
            }
            if (JS_IsObject(val))
            {
                auto* obj = new juce::DynamicObject();

                JSPropertyEnum* tab;
                uint32_t len;

                if (JS_GetOwnPropertyNames(ctx, &tab, &len, val, JS_GPN_STRING_MASK | JS_GPN_ENUM_ONLY) < 0)
                    throw std::runtime_error("AH");

                for (uint32_t i = 0; i < len; ++i)
                {
                    auto propVal = JS_GetProperty(ctx, val, tab[i].atom);
                    auto keyStr = JS_AtomToCString(ctx, tab[i].atom);
                    obj->setProperty(keyStr, JSValueToVar(ctx, propVal));

                    // TODO: Probably need to take much better care to do this all around
                    // this file.
                    JS_FreeValue(ctx, propVal);
                    JS_FreeCString(ctx, keyStr);
                }

                return juce::var(obj);
            }
            if (JS_IsException(val))
            {
                auto ex = JS_GetException(ctx);
                auto msg = JS_GetPropertyStr(ctx, ex, "message");
                auto stack = JS_GetPropertyStr(ctx, ex, "stack");

                throw EcmascriptEngine::Error(JS_ToCString(ctx, msg), JS_ToCString(ctx, stack));
            }

            DBG("Not sure what this is:");
            auto strVal = JS_ToCString(ctx, val);
            DBG(juce::String(strVal));
            JS_FreeCString(ctx, strVal);
            return juce::var();
        }

        //==============================================================================
        // TODO: unique_ptrs with custom deallocators
        // EDIT: shared_ptrs so that I can give a weak_ptr to the context in the native funcs
        // handed to the cpp user from the js environment
        JSRuntime* rt;
        JSContext* ctx;

        int nextMagicInt = 0;
        std::unordered_map<int, juce::var::NativeFunction> lambdaReleasePool;
        std::vector<std::unique_ptr<FuncTimer>> funcTimers;
    };

    //==============================================================================
    // Platform function implementations
    namespace
    {

        JSValue js_setTimeout(JSContext* ctx, JSValueConst thisVal, int argc, JSValueConst* argv)
        {
            if (auto* pimpl = static_cast<EcmascriptEngine::Pimpl*>(JS_GetContextOpaque(ctx)))
            {
                // TODO: Do this right
                return pimpl->setTimeout(ctx, thisVal, argc, argv);
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
        mPimpl->registerNativeProperty(name, value);
    }

    void EcmascriptEngine::registerNativeProperty (const juce::String& target, const juce::String& name, const juce::var& value)
    {
        mPimpl->registerNativeProperty(target, name, value);
    }

    //==============================================================================
    juce::var EcmascriptEngine::invoke (const juce::String& name, const std::vector<juce::var>& vargs)
    {
        return mPimpl->invoke(name, vargs);
    }

    void EcmascriptEngine::reset() {}

    //==============================================================================
    void EcmascriptEngine::debuggerAttach() {}
    void EcmascriptEngine::debuggerDetach() {}

}
