/*
  ==============================================================================

    EcmascriptEngine.cpp
    Created: 24 Oct 2019 3:08:39pm

  ==============================================================================
*/

/* We're careful to include the duktape source files before the module header
 * file because `duktape.c` sets certain preprocessor definitions that enable
 * necessary features in the duktape header. We need those defines to preempt
 * the loading of the duktape header. This also, therefore, is the place for
 * custom preprocessor definitions.
 *
 * We force Duktape to use a time provider on Windows that is compatible with
 * Windows 7 SP1. It looks like W7SP1 is quite happy with plugins built with
 * the 8.1 SDK, but the GetSystemTimePreciseAsFileTime() call used in here is
 * just not supported without the 8.1 dll available.
 */
#if defined (_WIN32) || defined (_WIN64)
#define DUK_USE_DATE_NOW_WINDOWS 1
#endif

/*
 * For whatever reason it is necessary to define this to resolve errors caused by both
 * duktape and juce including parts of the winsock2 API. There may be a better way to
 * resolve this.
 */
#if defined (_WIN32) || defined (_WIN64)
#define _WINSOCKAPI_
#endif

#if _MSC_VER
 #pragma warning(push)
#elif __clang__
 #pragma clang diagnostic push
 #pragma clang diagnostic ignored "-Wextra-semi"
 #pragma clang diagnostic ignored "-Wsign-conversion"
 #pragma clang diagnostic ignored "-Wswitch-enum"
 #pragma clang diagnostic ignored "-Wunused-parameter"
 #pragma clang diagnostic ignored "-Wused-but-marked-unused"
 #pragma clang diagnostic ignored "-Wformat-nonliteral"
 #pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
 #pragma clang diagnostic ignored "-Wshadow"
 #if __clang_major__ > 10
  #pragma clang diagnostic ignored "-Wc++98-compat-extra-semi"
  #pragma clang diagnostic ignored "-Wimplicit-int-conversion"
  #pragma clang diagnostic ignored "-Wshorten-64-to-32"
 #else
  #pragma clang diagnostic ignored "-Wconversion"
 #endif
#elif __GNUC__
 #pragma GCC diagnostic push
 #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
 #pragma GCC diagnostic ignored "-Wsign-conversion"
 #pragma GCC diagnostic ignored "-Wswitch-enum"
 #pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

// We rely on the JUCE_DEBUG macro in duk_config.h at the moment to determine
// when we enable duktape debug features. This is a bit of a hack to make this
// work. We should be able to do better and may do so once we enable custom duktape
// configs.
#include <juce_core/system/juce_TargetPlatform.h>

#include <duktape/src-noline/duktape.c>
#include <duktape/extras/console/duk_console.c>

#if defined (_WIN32) || defined (_WIN64)
    #include <duktape/examples/debug-trans-socket/duk_trans_socket_windows.c>
#else
    #include <duktape/examples/debug-trans-socket/duk_trans_socket_unix.c>
#endif

#include <duktape/src-noline/duktape.h>
#include <duktape/extras/console/duk_console.h>
#include <duktape/examples/debug-trans-socket/duk_trans_socket.h>

#if _MSC_VER
#elif __clang__
 #pragma clang diagnostic pop
#elif __GNUC__
 #pragma GCC diagnostic pop
#endif

#include "EcmascriptEngine.h"


namespace reactjuce
{

    namespace detail
    {

        static void fatalErrorHandler (void* udata, const char* msg)
        {
            (void) udata; // Ignored in this case, silence warning
            throw EcmascriptEngine::FatalError(msg);
        }

        static juce::String getContextDump(duk_context* ctx)
        {
            duk_push_context_dump(ctx);
            auto ret = juce::String(duk_to_string(ctx, -1));
            duk_pop(ctx);
            return ret;
        }

        static void safeCall(duk_context* ctx, const int numArgs)
        {
            if (duk_pcall(ctx, numArgs) != DUK_EXEC_SUCCESS)
            {
                const juce::String stack = duk_safe_to_stacktrace(ctx, -1);
                const juce::String msg = duk_safe_to_string(ctx, -1);

                throw EcmascriptEngine::Error(msg, stack, getContextDump(ctx));
            }
        }

        static void safeEvalString(duk_context* ctx, const juce::String& s)
        {
            if (duk_peval_string(ctx, s.toRawUTF8()) != DUK_EXEC_SUCCESS)
            {
                const juce::String stack = duk_safe_to_stacktrace(ctx, -1);
                const juce::String msg = duk_safe_to_string(ctx, -1);

                throw EcmascriptEngine::Error(msg, stack, getContextDump(ctx));
            }
        }

        static void safeCompileFile(duk_context* ctx, const juce::File& file)
        {
            auto name = file.getFileName();
            auto body = file.loadFileAsString();

            // Push the js filename to be compiled/evaluated
            duk_push_string(ctx, name.toRawUTF8());

            if (duk_pcompile_string_filename(ctx, DUK_COMPILE_EVAL, body.toRawUTF8()) != DUK_EXEC_SUCCESS)
            {
                const juce::String stack = duk_safe_to_stacktrace(ctx, -1);
                const juce::String msg = duk_safe_to_string(ctx, -1);

                throw EcmascriptEngine::Error(msg, stack, getContextDump(ctx));
            }
        }

    }

    //==============================================================================
    struct EcmascriptEngine::Pimpl : private juce::Timer
    {
        Pimpl() { reset(); }

        ~Pimpl() override
        {
            // NB: Explicitly stopping the timer so as to avoid any late calls to derefenrencing a (cleaned up) context.
            stopTimer();
        }

        //==============================================================================
        juce::var evaluateInline (const juce::String& code)
        {
            jassert(code.isNotEmpty());
            auto* ctxRawPtr = dukContext.get();

            try {
                detail::safeEvalString(ctxRawPtr, code);
            } catch (Error const& err) {
                reset();
                throw err;
            }

            auto result = readVarFromDukStack(dukContext, -1);
            duk_pop(ctxRawPtr);

            return result;
        }

        juce::var evaluate (const juce::File& code)
        {
            jassert(code.existsAsFile());
            jassert(code.loadFileAsString().isNotEmpty());
            auto* ctxRawPtr = dukContext.get();

            try {
                detail::safeCompileFile(ctxRawPtr, code);
                detail::safeCall(ctxRawPtr, 0);
            } catch (Error const& err) {
                reset();
                throw err;
            }

            // Collect the return value
            auto result = readVarFromDukStack(dukContext, -1);
            duk_pop(ctxRawPtr);

            return result;
        }

        //==============================================================================
        void registerNativeProperty (const juce::String& name, const juce::var& value)
        {
            auto* ctxRawPtr = dukContext.get();

            duk_push_global_object(ctxRawPtr);
            pushVarToDukStack(dukContext, value, true);
            duk_put_prop_string(ctxRawPtr, -2, name.toRawUTF8());
            duk_pop(ctxRawPtr);
        }

        void registerNativeProperty (const juce::String& target, const juce::String& name, const juce::var& value)
        {
            auto* ctxRawPtr = dukContext.get();

            try {
                detail::safeEvalString(ctxRawPtr, target);
            } catch (Error const& err) {
                reset();
                throw err;
            }

            // Then assign the property
            pushVarToDukStack(dukContext, value, true);
            duk_put_prop_string(ctxRawPtr, -2, name.toRawUTF8());
            duk_pop(ctxRawPtr);
        }

        //==============================================================================
        juce::var invoke (const juce::String& name, const std::vector<juce::var>& vargs)
        {
            auto* ctxRawPtr = dukContext.get();

            try {
                detail::safeEvalString(ctxRawPtr, name);

                if (!duk_is_function(ctxRawPtr, -1)) {
                    throw Error("Invocation failed, target is not a function.");
                }

                // Push the args to the duktape stack
                const auto nargs = static_cast<duk_idx_t>(vargs.size());
                duk_require_stack_top(ctxRawPtr, nargs);

                for (auto& p : vargs)
                    pushVarToDukStack(dukContext, p);

                // Invocation
                detail::safeCall(ctxRawPtr, nargs);
            } catch (Error const& err) {
                reset();
                throw err;
            }

            // Collect the return value
            auto result = readVarFromDukStack(dukContext, -1);
            duk_pop(ctxRawPtr);

            return result;
        }

        struct TimeoutFunctionManager : private juce::MultiTimer
        {
            ~TimeoutFunctionManager() override {
                for(const auto &[id, timer] : timeoutFunctions)
                    stopTimer(id);
            }

            juce::var clearTimeout(const int id)
            {
                stopTimer(id);
                const auto f = timeoutFunctions.find(id);
                if(f != timeoutFunctions.cend())
                    timeoutFunctions.erase(f);
                return juce::var();
            }

            int newTimeout(const juce::var::NativeFunction f, const int timeoutMillis, const std::vector<juce::var>&& args, const bool repeats=false)
            {
                static int nextId = 0;
                timeoutFunctions.emplace(nextId, TimeoutFunction(f, std::move(args), repeats));
                startTimer(nextId, timeoutMillis);
                return nextId++;
            }

            void timerCallback(int id) override
            {
                const auto f = timeoutFunctions.find(id);
                if(f != timeoutFunctions.cend())
                {
                    const auto cb = f->second;
                    std::invoke(cb.f, juce::var::NativeFunctionArgs(juce::var(), cb.args.data(), static_cast<int>(cb.args.size())));
                    if(!cb.repeats)
                    {
                        stopTimer(id);
                        timeoutFunctions.erase(f);
                    }
                }
            }

            private:
                struct TimeoutFunction
                {
                    TimeoutFunction(const juce::var::NativeFunction _f, const std::vector<juce::var> &&_args, const bool _repeats=false)
                    : f(_f), args(std::move(_args)), repeats(_repeats) {}

                    const juce::var::NativeFunction f;
                    std::vector<juce::var> args;
                    const bool repeats;
                };

                std::map<int, TimeoutFunction> timeoutFunctions;
        };

        // IsSetter is true for setTimeout / setInterval
        // and false for clearTimeout / clearInterval
        template <bool IsSetter = false, bool Repeats = false, typename MethodType>
        void registerNativeTimerFunction(const char* name, MethodType method)
        {
            registerNativeProperty(name, juce::var::NativeFunction([this, name, method] (const juce::var::NativeFunctionArgs& _args) -> juce::var {
                if constexpr (IsSetter)
                {
                    if(_args.numArguments < 2 || !_args.arguments[0].isMethod() || !_args.arguments[1].isDouble())
                        throw Error(juce::String(name) + " requires a callback and time in milliseconds");
                    // build a vector holding all additional arguments
                    std::vector<juce::var> args(_args.arguments + 2, _args.arguments + _args.numArguments);
                    return (this->timeoutsManager.get()->*method)(_args.arguments[0].getNativeFunction(), _args.arguments[1], std::move(args), Repeats);
                }
                else
                {
                    if(_args.numArguments < 1 || !_args.arguments[0].isDouble())
                        throw Error(juce::String(name) + " requires an integer ID of the timer to clear");
                    return (this->timeoutsManager.get()->*method)(_args.arguments[0]);
                }
            }));
        }

        void registerTimerGlobals()
        {
            registerNativeTimerFunction<true>(
                "setTimeout", &TimeoutFunctionManager::newTimeout
            );
            registerNativeTimerFunction<true, true>(
                "setInterval", &TimeoutFunctionManager::newTimeout
            );
            registerNativeTimerFunction("clearTimeout", &TimeoutFunctionManager::clearTimeout);
            registerNativeTimerFunction("clearInterval", &TimeoutFunctionManager::clearTimeout);
        }

        void reset()
        {
            // Clear out any timer callbacks
            timeoutsManager = std::make_unique<TimeoutFunctionManager>();

            // Allocate a new js heap
            dukContext = std::shared_ptr<duk_context>(
                duk_create_heap (nullptr, nullptr, nullptr, nullptr, detail::fatalErrorHandler),
                duk_destroy_heap
            );

            // Add console.log support
            auto* ctxRawPtr = dukContext.get();
            duk_console_init(ctxRawPtr, DUK_CONSOLE_FLUSH);

            // Install a pointer back to this EcmascriptEngine instance
            duk_push_global_stash(ctxRawPtr);
            duk_push_pointer(ctxRawPtr, (void *) this);
            duk_put_prop_string(ctxRawPtr, -2, DUK_HIDDEN_SYMBOL("__EcmascriptEngineInstance__"));
            duk_pop(ctxRawPtr);

            // Clear out any lambdas attached to the previous context instance
            persistentReleasePool.clear();

            // Register our various timeout-related native functions
            registerTimerGlobals();
        }

        //==============================================================================
        void debuggerAttach()
        {
            auto* ctxRawPtr = dukContext.get();

            duk_trans_socket_init();
            duk_trans_socket_waitconn();

            duk_debugger_attach(ctxRawPtr,
                duk_trans_socket_read_cb,
                duk_trans_socket_write_cb,
                duk_trans_socket_peek_cb,
                duk_trans_socket_read_flush_cb,
                duk_trans_socket_write_flush_cb,
                nullptr,
                [](duk_context*, void* data)
                {
                    duk_trans_socket_finish();

                    auto engine = static_cast<EcmascriptEngine::Pimpl*>(data);
                    engine->stopTimer();
                },
                this);

            // Start timer for duk_debugger_cooperate calls
            startTimer(200);
        }

        void debuggerDetach()
        {
            if (auto* dc = dukContext.get())
                duk_debugger_detach (dc);
        }

        //==============================================================================
        void timerCallback() override
        {
            if (auto* dc = dukContext.get())
                duk_debugger_cooperate (dc);
        }

        //==============================================================================
        struct LambdaHelper {
            LambdaHelper(juce::var::NativeFunction fn, uint32_t _id)
                : callback(std::move(fn)), id(_id) {}

            static duk_ret_t invokeFromDukContext(duk_context* ctx)
            {
                // First we have to retrieve the actual function pointer and our engine pointer
                // See: https://duktape.org/guide.html#hidden-symbol-properties
                duk_push_current_function(ctx);
                duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("LambdaHelperPtr"));

                LambdaHelper* helper = static_cast<LambdaHelper*>(duk_get_pointer(ctx, -1));
                duk_pop(ctx);

                // Then the engine...
                duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("EnginePtr"));
                auto* engine = static_cast<EcmascriptEngine::Pimpl*>(duk_get_pointer(ctx, -1));

                // Pop back both the pointer and the "current function"
                duk_pop_2(ctx);

                // Now we can collect our args
                std::vector<juce::var> args;
                int nargs = duk_get_top(ctx);

                for (int i = 0; i < nargs; ++i)
                    args.push_back(engine->readVarFromDukStack(engine->dukContext, i));

                juce::var result;

                // Now we can invoke the user method with its arguments
                try
                {
                    result = std::invoke(helper->callback, juce::var::NativeFunctionArgs(
                        juce::var(),
                        args.data(),
                        static_cast<int>(args.size())
                    ));
                }
                catch (Error& err)
                {
                    duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, err.what());
                    return duk_throw(ctx);
                }

                // For an undefined result, return 0 to notify the duktape interpreter
                if (result.isUndefined())
                    return 0;

                // Otherwise, push the result to the stack and tell duktape
                engine->pushVarToDukStack(engine->dukContext, result);
                return 1;
            }

            static duk_ret_t invokeFromDukContextLightFunc(duk_context* ctx)
            {
                // Retrieve the engine pointer
                duk_push_global_stash(ctx);
                duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("__EcmascriptEngineInstance__"));

                auto* engine = static_cast<EcmascriptEngine::Pimpl*>(duk_get_pointer(ctx, -1));
                duk_pop_2(ctx);

                // Retrieve the lambda helper
                duk_push_current_function(ctx);
                const auto magic = duk_get_magic(ctx, -1);
                auto& helper = engine->temporaryReleasePool[static_cast<size_t> (magic + 128)];
                duk_pop(ctx);

                // Now we can collect our args
                const auto nargs = duk_get_top(ctx);
                std::vector<juce::var> args;
                args.reserve(static_cast<size_t> (nargs));

                for (int i = 0; i < nargs; ++i)
                    args.push_back(engine->readVarFromDukStack(engine->dukContext, i));

                // Now we can invoke the user method with its arguments
                const auto result = std::invoke(helper->callback, juce::var::NativeFunctionArgs(
                    juce::var(),
                    args.data(),
                    static_cast<int>(args.size())
                ));

                // For an undefined result, return 0 to notify the duktape interpreter
                if (result.isUndefined())
                    return 0;

                // Otherwise, push the result to the stack and tell duktape
                engine->pushVarToDukStack(engine->dukContext, result);
                return 1;
            }

            static duk_ret_t callbackFinalizer (duk_context* ctx)
            {
                // First we have to retrieve the actual function pointer and our engine pointer
                // See: https://duktape.org/guide.html#hidden-symbol-properties
                // And: https://duktape.org/api.html#duk_set_finalizer
                // In this case our function is at index 0.
                duk_require_function(ctx, 0);
                duk_get_prop_string(ctx, 0, DUK_HIDDEN_SYMBOL("LambdaHelperPtr"));

                LambdaHelper* helper = static_cast<LambdaHelper*>(duk_get_pointer(ctx, -1));
                duk_pop(ctx);

                // Then the engine...
                duk_get_prop_string(ctx, 0, DUK_HIDDEN_SYMBOL("EnginePtr"));
                auto* engine = static_cast<EcmascriptEngine::Pimpl*>(duk_get_pointer(ctx, -1));

                // Pop back both the pointer and the "current function"
                duk_pop_2(ctx);

                // Clean up our lambda helper
                engine->removeLambdaHelper(helper);
                return 0;
            }

            juce::var::NativeFunction callback;
            uint32_t id;
        };

        //==============================================================================
        /** Helper for cleaning up native function temporaries. */
        void removeLambdaHelper (LambdaHelper* helper)
        {
            persistentReleasePool.erase(helper->id);
        }

        /** Helper for pushing a juce::var to the duktape stack. */
        void pushVarToDukStack (std::shared_ptr<duk_context> ctx, const juce::var& v, bool persistNativeFunctions = false)
        {
            auto* ctxRawPtr = dukContext.get();

            if (v.isVoid() || v.isUndefined())
                return duk_push_undefined(ctxRawPtr);
            if (v.isBool())
                return duk_push_boolean(ctxRawPtr, (bool) v);
            if (v.isInt() || v.isInt64())
                return duk_push_int(ctxRawPtr, (int) v);
            if (v.isDouble())
                return duk_push_number(ctxRawPtr, (double) v);
            if (v.isString())
                return (void) duk_push_string(ctxRawPtr, v.toString().toRawUTF8());
            if (v.isArray())
            {
                duk_idx_t arr_idx = duk_push_array(ctxRawPtr);
                duk_uarridx_t i = 0;

                for (auto& e : *(v.getArray()))
                {
                    pushVarToDukStack(ctx, e, persistNativeFunctions);
                    duk_put_prop_index(ctxRawPtr, arr_idx, i++);
                }

                return;
            }
            if (v.isObject())
            {
                if (auto* o = v.getDynamicObject())
                {
                    duk_idx_t obj_idx = duk_push_object(ctxRawPtr);

                    for (auto& e : o->getProperties())
                    {
                        pushVarToDukStack(ctx, e.value, persistNativeFunctions);
                        duk_put_prop_string(ctxRawPtr, obj_idx, e.name.toString().toRawUTF8());
                    }
                }

                return;
            }
            if (v.isMethod())
            {
                if (persistNativeFunctions)
                {
                    // For persisted native functions, we provide a helper layer storing and retrieving the
                    // stash, and marshalling between the Duktape C interface and the NativeFunction interface
                    duk_push_c_function(ctxRawPtr, LambdaHelper::invokeFromDukContext, DUK_VARARGS);

                    // Now we assign the pointers as properties of the wrapper function
                    auto helper = std::make_unique<LambdaHelper>(v.getNativeFunction(), nextHelperId++);
                    duk_push_pointer(ctxRawPtr, (void *) helper.get());
                    duk_put_prop_string(ctxRawPtr, -2, DUK_HIDDEN_SYMBOL("LambdaHelperPtr"));
                    duk_push_pointer(ctxRawPtr, (void *) this);
                    duk_put_prop_string(ctxRawPtr, -2, DUK_HIDDEN_SYMBOL("EnginePtr"));

                    // Now we prepare the finalizer
                    duk_push_c_function(ctxRawPtr, LambdaHelper::callbackFinalizer, 1);
                    duk_push_pointer(ctxRawPtr, (void *) helper.get());
                    duk_put_prop_string(ctxRawPtr, -2, DUK_HIDDEN_SYMBOL("LambdaHelperPtr"));
                    duk_push_pointer(ctxRawPtr, (void *) this);
                    duk_put_prop_string(ctxRawPtr, -2, DUK_HIDDEN_SYMBOL("EnginePtr"));
                    duk_set_finalizer(ctxRawPtr, -2);

                    // And hang on to it!
                    persistentReleasePool[helper->id] = std::move(helper);
                }
                else
                {
                    // For temporary native functions, we use the stack-allocated lightfunc. In
                    // this case we can't attach properties, so we can't rely on raw pointers to
                    // the LambdaHelper and we can't rely on finalizers. So, all we do here is use
                    // a small pool for temporary LambdaHelpers. Within this pool, we just allow insertions
                    // to wrap around and clobber previous temporaries, effectively garbage collecting on
                    // demand. The maximum number of temporary values before wrapping is 255, as dictated
                    // by that we use the lightfunc's magic number to identify our native callback.
                    auto helper = std::make_unique<LambdaHelper>(v.getNativeFunction(), nextHelperId++);
                    auto magic = nextMagicInt++;

                    duk_push_c_lightfunc(ctxRawPtr, LambdaHelper::invokeFromDukContextLightFunc, DUK_VARARGS, 15, magic);
                    temporaryReleasePool[static_cast<size_t> (magic + 128)] = std::move(helper);

                    if (nextMagicInt >= 127)
                        nextMagicInt = -128;
                }
                return;
            }

            // If you hit this, you tried to push an unsupported var type to the duktape
            // stack.
            jassertfalse;
        }

        /** Helper for reading from the duktape stack to a juce::var instance. */
        juce::var readVarFromDukStack (std::shared_ptr<duk_context> ctx, duk_idx_t idx)
        {
            auto* ctxRawPtr = dukContext.get();
            juce::var value;

            switch (duk_get_type(ctxRawPtr, idx))
            {
                case DUK_TYPE_NULL:
                    // It looks like juce::var doesn't have an explicit null value,
                    // so we're just using the default empty constructor value.
                    break;
                case DUK_TYPE_UNDEFINED:
                    value = juce::var::undefined();
                    break;
                case DUK_TYPE_BOOLEAN:
                    value = (bool) duk_get_boolean(ctxRawPtr, idx);
                    break;
                case DUK_TYPE_NUMBER:
                    value = duk_get_number(ctxRawPtr, idx);
                    break;
                case DUK_TYPE_STRING:
                    value = juce::String(juce::CharPointer_UTF8(duk_get_string(ctxRawPtr, idx)));
                    break;
                case DUK_TYPE_OBJECT:
                case DUK_TYPE_LIGHTFUNC:
                {
                    if (duk_is_array(ctxRawPtr, idx))
                    {
                        duk_size_t len = duk_get_length(ctxRawPtr, idx);
                        juce::Array<juce::var> els;

                        for (duk_size_t i = 0; i < len; ++i)
                        {
                            duk_get_prop_index(ctxRawPtr, idx, static_cast<duk_uarridx_t>(i));
                            els.add(readVarFromDukStack(ctx, -1));
                            duk_pop(ctxRawPtr);
                        }

                        value = els;
                        break;
                    }

                    if (duk_is_function(ctxRawPtr, idx) || duk_is_lightfunc(ctxRawPtr, idx))
                    {
                        struct CallbackHelper {
                            CallbackHelper(std::weak_ptr<duk_context> _weakContext)
                                : weakContext(_weakContext)
                                , funcId(juce::String("__NativeCallback__") + juce::Uuid().toString()) {}

                            ~CallbackHelper() {
                                if (auto spt = weakContext.lock()) {
                                    duk_push_global_stash(spt.get());
                                    duk_del_prop_string(spt.get(), -1, funcId.toRawUTF8());
                                    duk_pop(spt.get());
                                }
                            }

                            std::weak_ptr<duk_context> weakContext;
                            juce::String funcId;
                        };

                        // With a function, we first push the function reference to
                        // the Duktape global stash so we can read it later.
                        auto helper = std::make_shared<CallbackHelper>(ctx);

                        duk_push_global_stash(ctxRawPtr);
                        duk_dup(ctxRawPtr, idx);
                        duk_put_prop_string(ctxRawPtr, -2, helper->funcId.toRawUTF8());
                        duk_pop(ctxRawPtr);

                        // Next we create a var::NativeFunction that captures the function
                        // id and knows how to invoke it
                        value = juce::var::NativeFunction {
                            [this, weakContext = std::weak_ptr<duk_context>(ctx), helper](const juce::var::NativeFunctionArgs& args) -> juce::var {
                                auto sharedContext = weakContext.lock();

                                // If our context disappeared, we return early
                                if (!sharedContext)
                                    return juce::var();

                                auto* rawPtr = sharedContext.get();

                                // Here when we're being invoked we retrieve the callback function from
                                // the global stash and invoke it with the provided args.
                                duk_push_global_stash(rawPtr);
                                duk_get_prop_string(rawPtr, -1, helper->funcId.toRawUTF8());

                                if (!(duk_is_lightfunc(rawPtr, -1) || duk_is_function(rawPtr, -1)))
                                    throw Error("Global callback not found.", "", detail::getContextDump(rawPtr));

                                // Push the args to the duktape stack
                                duk_require_stack_top(rawPtr, args.numArguments);

                                for (int i = 0; i < args.numArguments; ++i)
                                    pushVarToDukStack(sharedContext, args.arguments[i]);

                                // Invocation
                                try {
                                    detail::safeCall(rawPtr, args.numArguments);
                                } catch (Error const& err) {
                                    reset();
                                    throw err;
                                }

                                // Clean the result and the stash off the top of the stack
                                duk_pop_2(rawPtr);

                                // Callbacks don't really need return args?
                                return juce::var();
                            }
                        };

                        break;
                    }

                    // If it's not a function or an array, it's a regular object.
                    auto* obj = new juce::DynamicObject();

                    // Generic object enumeration; `duk_enum` pushes an enumerator
                    // object to the top of the stack
                    duk_enum(ctxRawPtr, idx, DUK_ENUM_OWN_PROPERTIES_ONLY);

                    while (duk_next(ctxRawPtr, -1, 1))
                    {
                        // For each found key/value pair, `duk_enum` pushes the
                        // values to the top of the stack. So here the stack top
                        // is [ ... enum key value]. Enum is at -3, key at -2,
                        // value at -1 from the stack top.
                        // Note here that all keys in an ECMAScript object are of
                        // type string, even arrays, e.g. `myArr[0]` has an implicit
                        // conversion from number to string. Thus here, while constructing
                        // the DynamicObject, we take the `toString()` value for the key
                        // always.
                        obj->setProperty(duk_to_string(ctxRawPtr, -2), readVarFromDukStack(ctx, -1));

                        // Clear the key/value pair from the stack
                        duk_pop_2(ctxRawPtr);
                    }

                    // Pop the enumerator from the stack
                    duk_pop(ctxRawPtr);

                    value = juce::var(obj);
                    break;
                }
                case DUK_TYPE_NONE:
                default:
                    jassertfalse;
            }

            return value;
        }

        //==============================================================================
        uint32_t nextHelperId = 0;
        int32_t nextMagicInt = 0;
        std::unordered_map<uint32_t, std::unique_ptr<LambdaHelper>> persistentReleasePool;
        std::array<std::unique_ptr<LambdaHelper>, 255> temporaryReleasePool;
        std::unique_ptr<TimeoutFunctionManager> timeoutsManager;

        // The duk_context must be listed after the release pools so that it is destructed
        // before the pools. That way, as the duk_context is being freed and finalizing all
        // of our lambda helpers, our pools still exist for those code paths.
        std::shared_ptr<duk_context> dukContext;
    };

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

    EcmascriptEngine::~EcmascriptEngine()
    {
    }

    //==============================================================================
    juce::var EcmascriptEngine::evaluateInline (const juce::String& code)
    {
        return mPimpl->evaluateInline(code);
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

    void EcmascriptEngine::reset()
    {
        mPimpl->reset();
    }

    //==============================================================================
    void EcmascriptEngine::debuggerAttach()
    {
        mPimpl->debuggerAttach();
    }

    void EcmascriptEngine::debuggerDetach()
    {
        mPimpl->debuggerDetach();
    }

}
