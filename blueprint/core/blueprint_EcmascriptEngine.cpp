/*
  ==============================================================================

    blueprint_EcmascriptEngine.cpp
    Created: 24 Oct 2019 3:08:39pm

  ==============================================================================
*/


namespace blueprint
{

    namespace detail
    {

        /** TODO: This nativeMethodWrapper is on its way out and needs to be replaced
         *  with the LambdaHelper as used in pushVarToDukStack/readVarFromDukStack.
         *  The helper allows lambdas with capture groups, so we can skip the manual
         *  passing of stash in the `registerNativeMethod` hooks as well.
         */
        static duk_ret_t nativeMethodWrapper (duk_context* ctx)
        {
            // First we have to retrieve the actual function pointer and our engine pointer
            // See: https://duktape.org/guide.html#hidden-symbol-properties
            duk_push_current_function(ctx);
            duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("NativeFunctionPtr"));

            auto function = reinterpret_cast<EcmascriptEngine::NativeFunction>(duk_get_pointer(ctx, -1));
            duk_pop(ctx);

            // Then the engine...
            duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("EnginePtr"));
            auto* engine = static_cast<EcmascriptEngine*>(duk_get_pointer(ctx, -1));
            duk_pop(ctx);

            // Then the stash...
            duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("StashPtr"));
            auto* stash = duk_get_pointer(ctx, -1);

            // Pop back both the pointer and the "current function"
            duk_pop_2(ctx);

            // Now we can collect our args
            std::vector<juce::var> args;
            int nargs = duk_get_top(ctx);

            for (int i = 0; i < nargs; ++i)
                args.push_back(engine->readVarFromDukStack(ctx, i));

            // Now we can invoke the user method with its arguments
            auto result = function(stash, juce::var::NativeFunctionArgs(
                juce::var(),
                args.data(),
                static_cast<int>(args.size())
            ));

            // For an undefined result, return 0 to notify the duktape interpreter
            if (result.isUndefined())
                return 0;

            // Otherwise, push the result to the stack and tell duktape
            engine->pushVarToDukStack(ctx, result);
            return 1;
        }

        static void fatalErrorHandler (void* udata, const char* msg)
        {
            (void) udata;  // Ignored in this case, silence warning

            DBG("**Blueprint Fatal Error:** " << msg);
            DBG("It is now unsafe to execute further code in the EcmascriptEngine.");

            throw std::runtime_error(msg);
        }

        //TODO: Keep safeEvalString and safeCompileFile or make EcmascripEngine member funcs
        //      now that we have out errorHandle wrapper in EcmascriptEngine? Passing
        //      callback here feels a bit ambiguous in regards to popping the error
        //      off of the stack after invoking the error callback. Whose responsibility is it etc.
        template <typename T>
        static bool safeEvalString(duk_context* ctx, const juce::String& s, T&& callback)
        {
            if (duk_peval_string(ctx, s.toRawUTF8()) != 0)
            {
                if (callback != nullptr)
                {
                    // Call the user provided error handler and indicate failure
                    std::invoke(callback);
                    return false;
                }

                duk_throw_raw(ctx);
            }

            // Indicate success
            return true;
        }

        template<typename T>
        static bool safeCompileFile(duk_context* ctx, const juce::File& file, T&& callback)
        {
            // Push the js filename to be compiled/evaluated
            duk_push_string(ctx, file.getFileName().toRawUTF8());

            if (duk_pcompile_string_filename(ctx, DUK_COMPILE_EVAL, file.loadFileAsString().toRawUTF8()) != 0)
            {
                if (callback != nullptr)
                {
                    // Call the user provided error handler.js and indicate failure
                    std::invoke(callback);
                    return false;
                }

                duk_throw_raw(ctx);
            }

            // Indicate success
            return true;
       }

/*
       static void dumpContext(duk_context* ctx)
       {
           duk_push_context_dump(ctx);
           DBG(duk_to_string(ctx, -1));
           duk_pop(ctx);
       }
*/
    }

    //==============================================================================
    EcmascriptEngine::EcmascriptEngine()
    {
        // Allocate a new js heap
        ctx = duk_create_heap(NULL, NULL, NULL, NULL, detail::fatalErrorHandler);

        // Add console.log support
        duk_console_init(ctx, DUK_CONSOLE_FLUSH);

        // Create our error handler wrapping a user supplied error callback.
        errorHandler = [=]
        {
            if (onUncaughtError == nullptr)
                duk_throw_raw(ctx);

            const juce::String trace = duk_safe_to_stacktrace(ctx, -1);
            const juce::String msg = duk_safe_to_string(ctx, -1);

            // Call the user provided error handler
            std::invoke(onUncaughtError, msg, trace);

            // Clear out the stack so we can re-register native functions
            // after we clear out the lambda release pool etc.
            while (duk_get_top(ctx))
            {
                duk_remove(ctx, duk_get_top_index(ctx));
            }

            // Clear the LambdaHelper release pool as duktape does not call object
            // finalizers in the event of an evaluation error or duk_pcall failure.
            lambdaReleasePool.clear();
        };

    }

    EcmascriptEngine::~EcmascriptEngine()
    {
        duk_destroy_heap(ctx);
    }

    //==============================================================================
    juce::var EcmascriptEngine::evaluate (const juce::String& code)
    {
        jassert(code.isNotEmpty());

        if (!detail::safeEvalString(ctx, code, errorHandler))
            return juce::var(EvaluationError);

        auto result = readVarFromDukStack(ctx, -1);
        duk_pop(ctx);

        return result;
    }

    juce::var EcmascriptEngine::evaluate (const juce::File& code)
    {
        jassert(code.existsAsFile());
        jassert(code.loadFileAsString().isNotEmpty());

        juce::var result = juce::var::undefined();

        if (!detail::safeCompileFile(ctx, code, errorHandler))
            return juce::var(EvaluationError);

        // Call compiled function
        if (duk_pcall(ctx, 0) != DUK_EXEC_SUCCESS)
        {
            errorHandler();
            return juce::var(EvaluationError);
        }

        // Collect the return value
        result = readVarFromDukStack(ctx, -1);
        duk_pop(ctx);

        return result;
    }

    //==============================================================================
    void EcmascriptEngine::registerNativeMethod (const juce::String& name, NativeFunction fn, void* stash)
    {
        duk_push_global_object(ctx);

        // We wrap the native function to provide a helper layer storing and retrieving the
        // stash, and marshalling between the Duktape C interface and the NativeFunction interface
        duk_push_c_function(ctx, detail::nativeMethodWrapper, DUK_VARARGS);

        // Now we assign the pointers as properties of the wrapper function
        duk_push_pointer(ctx, (void *) fn);
        duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("NativeFunctionPtr"));
        duk_push_pointer(ctx, (void *) this);
        duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("EnginePtr"));
        duk_push_pointer(ctx, stash);
        duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("StashPtr"));

        // And finally we assign the function to its name in the global namespace
        duk_put_prop_string(ctx, -2, name.toRawUTF8());
    }

    void EcmascriptEngine::registerNativeMethod (const juce::String& target, const juce::String& name, NativeFunction fn, void* stash)
    {
        // Evaluate the target string on the context, leaving the result on the stack
        if (!detail::safeEvalString(ctx, target, errorHandler))
            return;

        // We wrap the native function to provide a helper layer storing and retrieving the
        // stash, and marshalling between the Duktape C interface and the NativeFunction interface
        duk_push_c_function(ctx, detail::nativeMethodWrapper, DUK_VARARGS);

        // Now we assign the pointers as properties of the wrapper function
        duk_push_pointer(ctx, (void *) fn);
        duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("NativeFunctionPtr"));
        duk_push_pointer(ctx, (void *) this);
        duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("EnginePtr"));
        duk_push_pointer(ctx, stash);
        duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("StashPtr"));

        // And finally we assign the function to its name in the global namespace
        duk_put_prop_string(ctx, -2, name.toRawUTF8());
    }

    //==============================================================================
    void EcmascriptEngine::registerNativeProperty (const juce::String& name, const juce::var& value)
    {
        duk_push_global_object(ctx);
        pushVarToDukStack(ctx, value);
        duk_put_prop_string(ctx, -2, name.toRawUTF8());
    }

    void EcmascriptEngine::registerNativeProperty (const juce::String& target, const juce::String& name, const juce::var& value)
    {
        // Evaluate the target string on the context, leaving the result on the stack
        // TODO: Return specific error code if target not in stack?
        if (!detail::safeEvalString(ctx, target, errorHandler))
            return;

        // Then assign the property
        pushVarToDukStack(ctx, value);
        duk_put_prop_string(ctx, -2, name.toRawUTF8());
    }

    //==============================================================================
    juce::var EcmascriptEngine::invoke (const juce::String& name, const std::vector<juce::var>& vargs)
    {
        // Evaluate the target string on the context, leaving the result on the stack
        // TODO: Return specific error code if name not in stack?
        if (!detail::safeEvalString(ctx, name, errorHandler))
            return juce::var::undefined();

        // Ensure requested function exists on the stack
        duk_require_function(ctx, -1);

        // Push the args to the duktape stack
        const auto nargs = static_cast<duk_idx_t>(vargs.size());
        duk_require_stack_top(ctx, nargs);

        for (auto& p : vargs)
            pushVarToDukStack(ctx, p);

        // Invocation
        if (duk_pcall(ctx, nargs) != DUK_EXEC_SUCCESS)
        {
            errorHandler();

            // TODO: Return specific error code if invoke fails?
            return juce::var::undefined();
        }

        // Collect the return value
        auto result = readVarFromDukStack(ctx, -1);
        duk_pop(ctx);

        return result;
    }

    //==============================================================================
    void EcmascriptEngine::debuggerAttach()
    {
        duk_trans_socket_init();
        duk_trans_socket_waitconn();

        duk_debugger_attach(ctx,
            duk_trans_socket_read_cb,
            duk_trans_socket_write_cb,
            duk_trans_socket_peek_cb,
            duk_trans_socket_read_flush_cb,
            duk_trans_socket_write_flush_cb,
            NULL,
            [](duk_context* ctx, void* data)
            {
                duk_trans_socket_finish();

                auto engine = static_cast<EcmascriptEngine*>(data);
                engine->stopTimer();
            },
            this);

        // Start timer for duk_debugger_cooperate calls
        startTimer(200);
    }

    void EcmascriptEngine::debuggerDetach()
    {
        duk_debugger_detach(ctx);
    }

    void EcmascriptEngine::timerCallback()
    {
        duk_debugger_cooperate(ctx);
    }

    //==============================================================================
    EcmascriptEngine::LambdaHelper::LambdaHelper(juce::var::NativeFunction fn)
        : callback(std::move(fn)) {}

    duk_ret_t EcmascriptEngine::LambdaHelper::invokeFromDukContext (duk_context* ctx)
    {
        // First we have to retrieve the actual function pointer and our engine pointer
        // See: https://duktape.org/guide.html#hidden-symbol-properties
        duk_push_current_function(ctx);
        duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("LambdaHelperPtr"));

        LambdaHelper* helper = static_cast<LambdaHelper*>(duk_get_pointer(ctx, -1));
        duk_pop(ctx);

        // Then the engine...
        duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("EnginePtr"));
        EcmascriptEngine* engine = static_cast<EcmascriptEngine*>(duk_get_pointer(ctx, -1));

        // Pop back both the pointer and the "current function"
        duk_pop_2(ctx);

        // Now we can collect our args
        std::vector<juce::var> args;
        int nargs = duk_get_top(ctx);

        for (int i = 0; i < nargs; ++i)
            args.push_back(engine->readVarFromDukStack(ctx, i));

        // Now we can invoke the user method with its arguments
        auto result = std::invoke(helper->callback, juce::var::NativeFunctionArgs(
            juce::var(),
            args.data(),
            static_cast<int>(args.size())
        ));

        // For an undefined result, return 0 to notify the duktape interpreter
        if (result.isUndefined())
            return 0;

        // Otherwise, push the result to the stack and tell duktape
        engine->pushVarToDukStack(ctx, result);
        return 1;
   }

    duk_ret_t EcmascriptEngine::LambdaHelper::callbackFinalizer (duk_context* ctx)
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
        EcmascriptEngine* engine = static_cast<EcmascriptEngine*>(duk_get_pointer(ctx, -1));

        // Pop back both the pointer and the "current function"
        duk_pop_2(ctx);

        // Clean up our lambda helper
        engine->removeLambdaHelper(helper);

        return 0;
    }

    //==============================================================================
    void EcmascriptEngine::removeLambdaHelper (LambdaHelper* helper)
    {
        lambdaReleasePool.erase(helper->id);
    }

    //==============================================================================
    /** Helper method to push a juce::var to the duktape stack. */
    void EcmascriptEngine::pushVarToDukStack (duk_context* ctx, const juce::var& v)
    {
        if (v.isVoid() || v.isUndefined())
            return duk_push_undefined(ctx);
        if (v.isBool())
            return duk_push_boolean(ctx, (bool) v);
        if (v.isInt() || v.isInt64())
            return duk_push_int(ctx, (int) v);
        if (v.isDouble())
            return duk_push_number(ctx, (double) v);
        if (v.isString())
            return (void) duk_push_string(ctx, v.toString().toRawUTF8());
        if (v.isArray())
        {
            duk_idx_t arr_idx = duk_push_array(ctx);
            int i = 0;

            for (auto& e : *(v.getArray()))
            {
                pushVarToDukStack(ctx, e);
                duk_put_prop_index(ctx, arr_idx, i++);
            }

            return;
        }
        if (v.isObject())
        {
            if (auto* o = v.getDynamicObject())
            {
                duk_idx_t obj_idx = duk_push_object(ctx);

                for (auto& e : o->getProperties())
                {
                    pushVarToDukStack(ctx, e.value);
                    duk_put_prop_string(ctx, obj_idx, e.name.toString().toRawUTF8());
                }
            }

            return;
        }
        if (v.isMethod())
        {
            // We wrap the native function to provide a helper layer storing and retrieving the
            // stash, and marshalling between the Duktape C interface and the NativeFunction interface
            duk_push_c_function(ctx, LambdaHelper::invokeFromDukContext, DUK_VARARGS);

            // Now we assign the pointers as properties of the wrapper function
            auto helper = std::make_unique<LambdaHelper>(v.getNativeFunction());
            duk_push_pointer(ctx, (void *) helper.get());
            duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("LambdaHelperPtr"));
            duk_push_pointer(ctx, (void *) this);
            duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("EnginePtr"));

            // Now we prepare the finalizer
            duk_push_c_function(ctx, LambdaHelper::callbackFinalizer, 1);
            duk_push_pointer(ctx, (void *) helper.get());
            duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("NativeFunctionPtr"));
            duk_push_pointer(ctx, (void *) this);
            duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("EnginePtr"));
            duk_set_finalizer(ctx, -2);

            // And hang on to it!
            lambdaReleasePool[helper->id] = std::move(helper);
            return;
        }

        // If you hit this, you tried to push an unsupported var type to the duktape
        // stack.
        jassertfalse;
    }

    /** Helper method to read a juce::var from the duktape stack. */
    juce::var EcmascriptEngine::readVarFromDukStack (duk_context* ctx, duk_idx_t idx)
    {
        juce::var value;

        switch (duk_get_type(ctx, idx))
        {
            case DUK_TYPE_NULL:
                // It looks like juce::var doesn't have an explicit null value,
                // so we're just using the default empty constructor value.
                break;
            case DUK_TYPE_UNDEFINED:
                value = juce::var::undefined();
                break;
            case DUK_TYPE_BOOLEAN:
                value = (bool) duk_get_boolean(ctx, idx);
                break;
            case DUK_TYPE_NUMBER:
                value = duk_get_number(ctx, idx);
                break;
            case DUK_TYPE_STRING:
                value = juce::String(juce::CharPointer_UTF8(duk_get_string(ctx, idx)));
                break;
            case DUK_TYPE_OBJECT:
            {
                if (duk_is_array(ctx, idx))
                {
                    duk_size_t len = duk_get_length(ctx, idx);
                    juce::Array<juce::var> els;

                    for (duk_size_t i = 0; i < len; ++i)
                    {
                        duk_get_prop_index(ctx, idx, i);
                        els.add(readVarFromDukStack(ctx, -1));
                        duk_pop(ctx);
                    }

                    value = els;
                    break;
                }

                if (duk_is_function(ctx, idx))
                {
                    // With a function, we first push the function reference to
                    // the Duktape global stash so we can read it later.
                    auto funId = juce::String("__blueprintCallback__") + juce::Uuid().toString();

                    duk_push_global_stash(ctx);
                    duk_dup(ctx, idx);
                    duk_put_prop_string(ctx, -2, funId.toRawUTF8());

                    // Next we create a var::NativeFunction that captures the function
                    // id and knows how to invoke it
                    value = juce::var::NativeFunction {
                        [this, ctx, funId = std::move(funId)](const juce::var::NativeFunctionArgs& args) -> juce::var {
                            // Here when we're being invoked we retrieve the callback function from
                            // the global stash and invoke it with the provided args.
                            duk_push_global_stash(ctx);
                            duk_get_prop_string(ctx, -1, funId.toRawUTF8());
                            duk_require_function(ctx, -1);

                            // Push the args to the duktape stack
                            duk_require_stack_top(ctx, args.numArguments);

                            for (int i = 0; i < args.numArguments; ++i)
                                pushVarToDukStack(ctx, args.arguments[i]);

                            // Invocation
                            if (duk_pcall(ctx, args.numArguments) != DUK_EXEC_SUCCESS)
                                duk_throw(ctx);

                            // Clean the result and the stash off the top of the stack
                            duk_pop_2(ctx);

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
                duk_enum(ctx, idx, DUK_ENUM_OWN_PROPERTIES_ONLY);

                while (duk_next(ctx, -1, 1))
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
                    obj->setProperty(duk_to_string(ctx, -2), readVarFromDukStack(ctx, -1));

                    // Clear the key/value pair from the stack
                    duk_pop_2(ctx);
                }

                // Pop the enumerator from the stack
                duk_pop(ctx);

                value = juce::var(obj);
                break;
            }
            case DUK_TYPE_NONE:
            default:
                jassertfalse;
        }

        return value;
    }

}
