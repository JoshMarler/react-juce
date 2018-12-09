/*
  ==============================================================================

    blueprint_ReactApplicationRoot.h
    Created: 9 Dec 2018 10:20:37am

  ==============================================================================
*/

#pragma once


namespace blueprint
{

    //==============================================================================
    /** The ReactApplicationRoot class prepares and maintains a Duktape evaluation
        context with the relevant hooks for supporting the Blueprint render
        backend.
     */
    class ReactApplicationRoot
    {
    public:
        //==============================================================================
        ReactApplicationRoot()
        {
            ctx = duk_create_heap_default();
            duk_console_init(ctx, DUK_CONSOLE_FLUSH);
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
