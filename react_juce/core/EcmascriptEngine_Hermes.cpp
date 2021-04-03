#include "EcmascriptEngine.h"

#if _MSC_VER
#pragma warning(push)
#elif __clang__
#pragma clang diagnostic push
 #pragma clang diagnostic ignored "-Winconsistent-missing-destructor-override"
 #pragma clang diagnostic ignored "-Wextra-semi"
 #pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#elif __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

#include <hermes/hermes.h>

#if _MSC_VER
#elif __clang__
#pragma clang diagnostic pop
#elif __GNUC__
#pragma GCC diagnostic pop
#endif

using namespace facebook;

namespace reactjuce
{
    namespace
    {
        //==============================================================================
        class JSIMemoryBuffer : public jsi::Buffer
        {
        public:
            explicit JSIMemoryBuffer(std::unique_ptr<juce::MemoryBlock> mem)
                : memBlock(std::move(mem))
            { }

            size_t size() const override
            {
                return memBlock->getSize();
            }

            const uint8_t *data() const override
            {
                return reinterpret_cast<const uint8_t*>(memBlock->getData());
            }

        private:
            std::unique_ptr<juce::MemoryBlock> memBlock;
        };

        //==============================================================================
        struct CopyableJSIMethodWrapper
        {
            explicit CopyableJSIMethodWrapper(jsi::Function f)
                : fn(std::move(f))
            { }

            jsi::Function fn;
        };

        //==============================================================================
        juce::var  jsiValueToVar(const jsi::Value &v, jsi::Runtime &runtime);
        jsi::Value varToJSIValue(const juce::var &v, jsi::Runtime &runtime);

        juce::var jsiArrayToVarArray(const jsi::Array &v, jsi::Runtime &runtime)
        {
            juce::Array<juce::var> varArray;

            const size_t numItems = v.size(runtime);
            for (size_t i = 0; i < numItems; ++i)
            {
                varArray.add(jsiValueToVar(v.getValueAtIndex(runtime, i), runtime));
            }

            return varArray;
        }

        juce::var::NativeFunction jsiMethodToVarMethod(jsi::Function v, jsi::Runtime &runtime)
        {
            auto fPtr = std::make_shared<CopyableJSIMethodWrapper>(std::move(v));

            return [fPtr = fPtr, &rt = runtime] (const juce::var::NativeFunctionArgs &args)
            {
                std::vector<jsi::Value> jsiArgs;
                for (int i = 0; i < args.numArguments; ++i)
                {
                    jsiArgs.emplace_back(varToJSIValue(args.arguments[i], rt));
                }

                const jsi::Value *jsiArgsPtr = jsiArgs.data();
                return jsiValueToVar(fPtr->fn.call(rt, jsiArgsPtr, jsiArgs.size()), rt);
            };
        }

        juce::var jsiObjectToVarObject(const jsi::Object &v, jsi::Runtime &runtime)
        {
            juce::DynamicObject::Ptr varObj = new juce::DynamicObject();

            jsi::Array props = v.getPropertyNames(runtime);
            for (size_t i = 0; i < props.size(runtime); ++i)
            {
                const juce::String propId = props.getValueAtIndex(runtime, i).asString(runtime).utf8(runtime);
                jsi::Value property = v.getProperty(runtime, propId.toRawUTF8());
                varObj->setProperty(propId, jsiValueToVar(property, runtime));
            }

            return varObj.get();
        }

        juce::var jsiValueToVar(const jsi::Value &v, jsi::Runtime &runtime)
        {
            if (v.isBool())
                return v.getBool();

            if (v.isNumber())
                return v.getNumber();

            if (v.isString())
                return juce::String(v.getString(runtime).utf8(runtime));

            if (v.isUndefined())
                return juce::var::undefined();

            if (v.isNull())
                return juce::var();

            if (v.isObject())
            {
                auto obj = v.getObject(runtime);

                if (obj.isArray(runtime))
                    return jsiArrayToVarArray(obj.getArray(runtime), runtime);

                if (obj.isArrayBuffer(runtime))
                {
                    jsi::ArrayBuffer arrayBuffer = obj.getArrayBuffer(runtime);
                    juce::MemoryBlock memBlock(static_cast<const void*>(arrayBuffer.data(runtime)),
                                               arrayBuffer.size(runtime));

                    return juce::var(memBlock);
                }

                if (obj.isFunction(runtime))
                    return jsiMethodToVarMethod(obj.asFunction(runtime), runtime);

                // If jsi object is not array or function type then treat as an object
                return jsiObjectToVarObject(obj, runtime);
            }

            jassertfalse;
            return {};
        }

        //==============================================================================
        jsi::Object varObjectToJSIObject(juce::DynamicObject *v, jsi::Runtime &runtime)
        {
            jassert(v);

            jsi::Object jsiObj(runtime);
            for (auto &prop : v->getProperties())
            {
                jsiObj.setProperty(runtime, prop.name.getCharPointer(), varToJSIValue(prop.value, runtime));
            }

            return jsiObj;
        }

        jsi::Function varMethodToJSIMethod(juce::var::NativeFunction &v, jsi::Runtime &runtime)
        {
            return jsi::Function::createFromHostFunction(
                runtime,
                jsi::PropNameID::forAscii(runtime, ""),
                0,
                [v = v](jsi::Runtime& rt, const jsi::Value& thisVal, const jsi::Value* args, size_t count)
                {
                    std::vector<juce::var> varArgs;
                    for (size_t i = 0; i < count; ++i)
                    {
                        varArgs.push_back(jsiValueToVar(args[i], rt));
                    }

                    juce::var::NativeFunctionArgs nfArgs(jsiValueToVar(thisVal, rt), varArgs.data(), static_cast<int>(count));
                    return varToJSIValue(v(nfArgs), rt);
                }
            );
        }

        jsi::Array varArrayToJSIArray(const juce::Array<juce::var> *v, jsi::Runtime &runtime)
        {
            jassert(v);

            const auto numItems = static_cast<size_t>(v->size());
            jsi::Array jsiArray(runtime, numItems);

            for (size_t i = 0; i < numItems; ++i)
            {
                jsiArray.setValueAtIndex(runtime, i, varToJSIValue(v->getReference(static_cast<int>(i)), runtime));
            }

            return jsiArray;
        }

        jsi::Value varToJSIValue(const juce::var &v, jsi::Runtime &runtime)
        {
            if (v.isBool())
                return jsi::Value(static_cast<bool>(v));

            if (v.isInt())
                return jsi::Value(static_cast<int>(v));

            if (v.isInt64())
                return jsi::Value(static_cast<double>(v));

            if (v.isDouble())
                return jsi::Value(static_cast<double>(v));

            if (v.isString())
                return jsi::Value(runtime,  jsi::String::createFromAscii(runtime, v.toString().getCharPointer()));

            if (v.isUndefined())
                return jsi::Value::undefined();

            if (v.isVoid())
                return {};

            if (v.isArray())
                return varArrayToJSIArray(v.getArray(), runtime);

            if (v.isMethod())
            {
                auto nf = v.getNativeFunction();
                return varMethodToJSIMethod(nf, runtime);
            }

            if (v.isObject())
                return varObjectToJSIObject(v.getDynamicObject(), runtime);

            if (v.isBinaryData())
            {
                //TODO: Currently appears no way to create a jsi::ArrayBuffer from C++.
                jassertfalse;
                return {};
            }

            jassertfalse;
            return {};
        }

        //==============================================================================
    }

    //==============================================================================
    struct EcmascriptEngine::Pimpl
    {
        //==============================================================================
        struct TimeoutFunctionManager : private juce::MultiTimer
        {
            explicit TimeoutFunctionManager(jsi::Runtime &rt)
                : runtime(rt)
            { }

            ~TimeoutFunctionManager() override
            {
                clear();
            }

            void clear()
            {
                std::for_each(timeoutFunctions.cbegin(), timeoutFunctions.cend(), [this] (const auto &tf)
                {
                    stopTimer(tf.first);
                });

                timeoutFunctions.clear();
            }

            jsi::Value clearTimeout(const int id)
            {
                stopTimer(id);

                if (const auto f = timeoutFunctions.find(id); f != timeoutFunctions.cend())
                    timeoutFunctions.erase(f);

                return jsi::Value();
            }

            jsi::Value newTimeout(jsi::Function f, const int timeoutMillis, std::vector<jsi::Value> args, const bool repeats=false)
            {
                static int nextId = 0;
                timeoutFunctions.emplace(nextId, TimeoutFunction(std::move(f), std::move(args), repeats));
                startTimer(nextId, timeoutMillis);

                return nextId++;
            }

            void timerCallback(int id) override
            {
                if (const auto f = timeoutFunctions.find(id); f != timeoutFunctions.cend())
                {
                    TimeoutFunction &cb = f->second;

                    const jsi::Value *argsPtr = cb.args.data();
                    cb.f.call(runtime, argsPtr, cb.args.size());

                    if (!cb.repeats)
                    {
                        stopTimer(id);
                        timeoutFunctions.erase(f);
                    }
                }
            }

        private:
            struct TimeoutFunction
            {
                TimeoutFunction(jsi::Function _f, std::vector<jsi::Value> _args, const bool _repeats=false)
                    : f(std::move(_f))
                    , args(std::move(_args))
                    , repeats(_repeats)
                { }

                jsi::Function           f;
                std::vector<jsi::Value> args;
                const bool              repeats;
            };

            jsi::Runtime                   &runtime;
            std::map<int, TimeoutFunction>  timeoutFunctions;
        };

        //==============================================================================
        Pimpl()
        {
            reset();
        }

        ~Pimpl() = default;

        //==============================================================================
        juce::var evaluateInline(const juce::String &code)
        {
            try
            {
                auto jsiBuffer = std::make_shared<jsi::StringBuffer>(code.toStdString());
                auto js        = runtime->prepareJavaScript(jsiBuffer, "");
                auto result    = runtime->evaluatePreparedJavaScript(js);

                return jsiValueToVar(result, *runtime);
            }
            catch (const jsi::JSIException &e)
            {
                throw Error(e.what());
            }
        }

        juce::var evaluate(const juce::File &code)
        {
            try
            {
                auto jsiBuffer = std::make_shared<jsi::StringBuffer>(code.loadFileAsString().toStdString());
                auto js        = runtime->prepareJavaScript(jsiBuffer, code.getFullPathName().toStdString());
                auto result    = runtime->evaluatePreparedJavaScript(js);

                return jsiValueToVar(result, *runtime);
            }
            catch (const jsi::JSIException &e)
            {
                throw Error(e.what());
            }
        }

        juce::var evaluateBytecode(const juce::File &code)
        {
            try
            {
                auto memBlock = std::make_unique<juce::MemoryBlock>();
                code.loadFileAsData(*memBlock);

                auto jsiBuffer = std::make_shared<JSIMemoryBuffer>(std::move(memBlock));
                auto js        = runtime->prepareJavaScript(jsiBuffer, code.getFullPathName().toStdString());
                auto result    = runtime->evaluatePreparedJavaScript(js);

                return jsiValueToVar(result, *runtime);
            }
            catch (const jsi::JSIException &e)
            {
                throw Error(e.what());
            }
        }

        //==============================================================================
        void registerNativeProperty(const juce::String &name, const juce::var &value)
        {
            try
            {
                runtime->global().setProperty(*runtime, name.toRawUTF8(), varToJSIValue(value, *runtime));
            }
            catch (const jsi::JSIException &e)
            {
                throw Error(e.what());
            }
        }

        void registerNativeProperty(const juce::String &target, const juce::String &name, const juce::var &value)
        {
            try
            {
                auto obj = runtime->global().getPropertyAsObject(*runtime, target.toRawUTF8());
                obj.setProperty(*runtime, name.toRawUTF8(), varToJSIValue(value, *runtime));
            }
            catch (const jsi::JSIException &e)
            {
                throw Error(e.what());
            }
        }

        //==============================================================================
        //TODO: We can probably improve on this by refining the invoke API
        juce::var invoke(const juce::String &name, const std::vector<juce::var> &vargs)
        {
            try
            {
                juce::StringArray accessors;
                accessors.addTokens(name.trim(), ".", "");
                accessors.removeEmptyStrings();

                jsi::Value prop = runtime->global();

                for (auto &p : accessors)
                {
                    if (prop.isObject())
                    {
                        jsi::Object obj = prop.getObject(*runtime);
                        prop = obj.getProperty(*runtime, p.toRawUTF8());
                    }
                }

                auto func = prop.getObject(*runtime).asFunction(*runtime);

                std::vector<jsi::Value> jsiArgs;
                for (auto &v : vargs)
                {
                    jsiArgs.push_back(varToJSIValue(v, *runtime));
                }

                const jsi::Value *argsPtr = jsiArgs.data();
                jsi::Value result = func.call(*runtime, argsPtr, jsiArgs.size());

                return jsiValueToVar(result, *runtime);
            }
            catch(const jsi::JSIException &e)
            {
                throw Error(e.what());
            }
        }

        //==============================================================================
        jsi::Function createSetTimerFunction(bool isInterval)
        {
            return jsi::Function::createFromHostFunction(
                *runtime,
                jsi::PropNameID::forAscii(*runtime, ""),
                0,
                [=] (jsi::Runtime& rt, const jsi::Value& thisVal, const jsi::Value* args, size_t count)
                {
                    juce::ignoreUnused(thisVal);

                    jsi::Function fn      = args[0].asObject(rt).asFunction(rt);
                    const auto    timeout = static_cast<int>(args[1].asNumber());

                    std::vector<jsi::Value> timeoutArgs;
                    if (count > 2)
                    {
                        for (size_t i = 2; i < count; ++i)
                        {
                            timeoutArgs.emplace_back(jsi::Value(rt, args[i]));
                        }
                    }

                    return timeoutsManager->newTimeout(std::move(fn), timeout, std::move(timeoutArgs), isInterval);
                }
            );
        }

        jsi::Function createClearTimerFunction()
        {
            return jsi::Function::createFromHostFunction(
                *runtime,
                jsi::PropNameID::forAscii(*runtime, ""),
                0,
                [this] (jsi::Runtime& rt, const jsi::Value& thisVal, const jsi::Value* args, size_t count)
                {
                    juce::ignoreUnused(rt);
                    juce::ignoreUnused(thisVal);
                    juce::ignoreUnused(count);

                    const auto timerId = static_cast<int>(args[0].asNumber());
                    return timeoutsManager->clearTimeout(timerId);
                }
            );
        }

        //==============================================================================
        void reset()
        {
            if (timeoutsManager)
                timeoutsManager->clear();

            runtime         = facebook::hermes::makeHermesRuntime();
            timeoutsManager = std::make_unique<TimeoutFunctionManager>(*runtime);

            // Quick and dirty console object provide. Could be improved upon.
            jsi::Function logFunction =
                jsi::Function::createFromHostFunction(
                    *runtime,
                    jsi::PropNameID::forAscii(*runtime, ""),
                    0,
                    [] (jsi::Runtime& rt, const jsi::Value& thisVal, const jsi::Value* args, size_t count)
                    {
                        juce::ignoreUnused(thisVal);
                        juce::String logString;

                        for (size_t i = 0; i < count; ++i)
                        {
                            auto s = args[i].toString(rt).utf8(rt);
                            logString.append(s, s.size());
                            logString.append(" ", 1u);
                        }

                        juce::Logger::writeToLog(logString);
                        return jsi::Value();
                    }
                );

            auto console = jsi::Object(*runtime);
            console.setProperty(*runtime, "log", logFunction);

            runtime->global().setProperty(*runtime, "console"      , console);
            runtime->global().setProperty(*runtime, "setTimeout"   , createSetTimerFunction(false));
            runtime->global().setProperty(*runtime, "setInterval"  , createSetTimerFunction(true));
            runtime->global().setProperty(*runtime, "clearTimeout" , createClearTimerFunction());
            runtime->global().setProperty(*runtime, "clearInterval", createClearTimerFunction());
        }

        void debuggerAttach()
        {
            //TODO: Implement Hermed debug support
            jassertfalse;
        }

        void debuggerDetach()
        {
            //TODO: Implement Hermed debug support
            jassertfalse;
        }

        std::unique_ptr<TimeoutFunctionManager>          timeoutsManager;
        std::unique_ptr<facebook::hermes::HermesRuntime> runtime;
    };

    //==============================================================================

}
