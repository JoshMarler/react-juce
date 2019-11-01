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

        /** Helper method to push a juce::var to the duktape stack. */
        static void pushVarToDukStack (duk_context* ctx, const juce::var& v)
        {
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

            // If you hit this, you tried to push an unsupported var type to the duktape
            // stack.
            jassertfalse;
        }

        /** Helper method to read a juce::var from the duktape stack. */
        static juce::var readVarFromDukStack (duk_context* ctx, duk_idx_t idx)
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
                    else
                    {
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
                }
                case DUK_TYPE_NONE:
                default:
                    jassertfalse;
            }

            return value;
        }

        /** All of our native functions registered in the Duktape environment are
         *  proxied to by this wrapper. We actually register this wrapper function
         *  with Duktape but set properties on the function pointing back to the
         *  actual user function and user stash. This allows the opportunity to
         *  marshal to and from a JUCE API cleanly.
         */
        static duk_ret_t nativeMethodWrapper (duk_context* ctx)
        {
            // First we collect the arguments from the stack
            std::vector<juce::var> args;
            int nargs = duk_get_top(ctx);

            for (int i = 0; i < nargs; ++i)
                args.push_back(detail::readVarFromDukStack(ctx, i));

            // Now we have to retrieve the actual function pointer and our stash pointer
            // See: https://duktape.org/guide.html#hidden-symbol-properties
            duk_push_current_function(ctx);
            duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("NativeFunctionPtr"));

            // Pull the function pointer from the top of the stack and then pop the
            // value from the stack
            EcmascriptEngine::NativeFunction f = reinterpret_cast<EcmascriptEngine::NativeFunction>(duk_get_pointer(ctx, -1));
            duk_pop(ctx);

            duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("StashPtr"));

            // Pull the stash pointer from the top of the stack, then pop both the
            // pointer and the `this` value from the stack
            void* stash = duk_get_pointer(ctx, -1);
            duk_pop_2(ctx);

            // Now we can invoke the user method with its arguments
            juce::var result = f(stash, juce::var::NativeFunctionArgs(
                juce::var(),
                args.data(),
                static_cast<int>(args.size())
            ));

            // For an undefined result, return 0 to notify the duktape interpreter
            if (result.isUndefined())
                return 0;

            // Otherwise, push the result to the stack and tell duktape
            detail::pushVarToDukStack(ctx, result);
            return 1;
        }

    }

    //==============================================================================
    EcmascriptEngine::EcmascriptEngine()
    {
        // Allocate a new js heap
        ctx = duk_create_heap_default();

        // Add console.log support
        duk_console_init(ctx, DUK_CONSOLE_FLUSH);
    }

    EcmascriptEngine::~EcmascriptEngine()
    {
        duk_destroy_heap(ctx);
    }

    //==============================================================================
    juce::Result EcmascriptEngine::execute (const juce::String& code)
    {
        duk_push_string(ctx, code.toRawUTF8());

        const duk_int_t rc = duk_peval(ctx);

        auto result = rc == 0
            ? juce::Result::ok()
            : juce::Result::fail(duk_safe_to_string(ctx, -1));

        duk_pop(ctx);
        return result;
    }

    juce::var EcmascriptEngine::evaluate (const juce::String& code)
    {
        duk_push_string(ctx, code.toRawUTF8());
        duk_eval(ctx);

        auto result = detail::readVarFromDukStack(ctx, -1);

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
        duk_push_pointer(ctx, stash);
        duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("StashPtr"));

        // And finally we assign the function to its name in the global namespace
        duk_put_prop_string(ctx, -2, name.toRawUTF8());
    }

    void EcmascriptEngine::registerNativeMethod (const juce::String& target, const juce::String& name, NativeFunction fn, void* stash)
    {
        // Evaluate the target string on the context, leaving the result on the stack
        duk_eval_string(ctx, target.toRawUTF8());

        // We wrap the native function to provide a helper layer storing and retrieving the
        // stash, and marshalling between the Duktape C interface and the NativeFunction interface
        duk_push_c_function(ctx, detail::nativeMethodWrapper, DUK_VARARGS);

        // Now we assign the pointers as properties of the wrapper function
        duk_push_pointer(ctx, (void *) fn);
        duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("NativeFunctionPtr"));
        duk_push_pointer(ctx, stash);
        duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("StashPtr"));

        // And finally we assign the function to its name in the global namespace
        duk_put_prop_string(ctx, -2, name.toRawUTF8());
    }

    //==============================================================================
    void EcmascriptEngine::registerNativeProperty (const juce::String& name, const juce::var& value)
    {
        duk_push_global_object(ctx);
        detail::pushVarToDukStack(ctx, value);
        duk_put_prop_string(ctx, -2, name.toRawUTF8());
    }

    void EcmascriptEngine::registerNativeProperty (const juce::String& target, const juce::String& name, const juce::var& value)
    {
        // Evaluate the target string on the context, leaving the result on the stack
        duk_eval_string(ctx, target.toRawUTF8());

        // Then assign the property
        detail::pushVarToDukStack(ctx, value);
        duk_put_prop_string(ctx, -2, name.toRawUTF8());
    }

    //==============================================================================
    juce::var EcmascriptEngine::invoke (const juce::String& name, const std::vector<juce::var>& vargs)
    {
        // Evaluate the target string on the context, leaving the result on the stack
        duk_eval_string(ctx, name.toRawUTF8());
        duk_require_function(ctx, -1);

        // Push the args to the duktape stack
        auto nargs = static_cast<duk_idx_t>(vargs.size());
        duk_require_stack_top(ctx, nargs);

        for (auto& p : vargs)
            detail::pushVarToDukStack(ctx, p);

        // Invocation
        duk_call(ctx, nargs);

        // Collect the return value
        auto result = detail::readVarFromDukStack(ctx, -1);

        duk_pop(ctx);
        return result;
    }

}
