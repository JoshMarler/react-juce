/*
  ==============================================================================

    blueprint_ReactApplicationRoot.h
    Created: 9 Dec 2018 10:20:37am

  ==============================================================================
*/

#pragma once


namespace blueprint
{

    duk_ret_t blueprint_create_instance(duk_context *ctx);

    //==============================================================================
    /** The ReactApplicationRoot class prepares and maintains a Duktape evaluation
        context with the relevant hooks for supporting the Blueprint render
        backend.
     */
    class ReactApplicationRoot
    {
    public:
        //==============================================================================
        // TODO: This is a bad way to do this. Basically we either need to store a pointer
        // to the ReactApplicationRoot instance associated with a given Duktape context
        // either in some global static map or by storing that pointer somewhere in user
        // data inside of the Duktape context.
        static ReactApplicationRoot* singletonInstance;

        //==============================================================================
        ReactApplicationRoot()
        {
            // See note above. Currently can only create one instance.
            jassert (singletonInstance == nullptr);
            singletonInstance = this;

            // Allocate a new js heap
            ctx = duk_create_heap_default();

            // Add console.log support
            duk_console_init(ctx, DUK_CONSOLE_FLUSH);

            // Register react render backend functions
            const duk_function_list_entry my_module_funcs[] = {
                { "createInstance", blueprint_create_instance, 1},
                { NULL, NULL, 0 }
            };

            duk_push_global_object(ctx);
            duk_push_object(ctx);
            duk_put_function_list(ctx, -1, my_module_funcs);
            duk_put_prop_string(ctx, -2, "BlueprintBackend");
            duk_pop(ctx);
        }

        ~ReactApplicationRoot()
        {
            duk_destroy_heap(ctx);
        }

        //==============================================================================
        void setRootComponent (juce::Component* root)
        {
            rootComponent = root;
        }

        void runScript (const juce::File& f)
        {
            auto src = f.loadFileAsString();

            duk_push_string(ctx, src.toRawUTF8());

            if (duk_peval(ctx) != 0) {
                printf("Script evaluation failed: %s\n", duk_safe_to_string(ctx, -1));
            }

            duk_pop(ctx);
        }

    private:
        //==============================================================================
        juce::Component* rootComponent;
        duk_context* ctx;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReactApplicationRoot)
    };

}
