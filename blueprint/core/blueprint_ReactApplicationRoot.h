/*
  ==============================================================================

    blueprint_ReactApplicationRoot.h
    Created: 9 Dec 2018 10:20:37am

  ==============================================================================
*/

#pragma once

#include <map>

#include "blueprint_View.h"


namespace blueprint
{

    /** This struct defines the set of functions that form the native interface
        for the JavaScript evaluation context.
     */
    struct BlueprintNative
    {
        static duk_ret_t createViewInstance (duk_context *ctx);
        static duk_ret_t setViewProperty (duk_context *ctx);
        static duk_ret_t appendChild (duk_context *ctx);
        static duk_ret_t getRootInstanceId (duk_context *ctx);
    };

    //==============================================================================
    /** The ReactApplicationRoot class prepares and maintains a Duktape evaluation
        context with the relevant hooks for supporting the Blueprint render
        backend.
     */
    class ReactApplicationRoot : public View
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
            const duk_function_list_entry blueprintNativeFuncs[] = {
                { "createViewInstance", BlueprintNative::createViewInstance, 1},
                { "setViewProperty", BlueprintNative::setViewProperty, 3},
                { "appendChild", BlueprintNative::appendChild, 2},
                { "getRootInstanceId", BlueprintNative::getRootInstanceId, 0},
                { NULL, NULL, 0 }
            };

            duk_push_global_object(ctx);
            duk_push_object(ctx);
            duk_put_function_list(ctx, -1, blueprintNativeFuncs);
            duk_put_prop_string(ctx, -2, "__BlueprintNative__");
            duk_pop(ctx);

            // Assign our own component id
            juce::Uuid id;
            juce::String sid = id.toDashedString();

            setComponentID(sid);
        }

        ~ReactApplicationRoot()
        {
            duk_destroy_heap(ctx);
        }

        //==============================================================================
        /** Override the default View behavior. */
        void resized() override
        {
            // When the top level React root receives a `resized()` event, we have to
            // recalculate the whole flex tree because of properties like flex-wrap which
            // depend on the available parent bounds.
            juce::Rectangle<float> bounds = getLocalBounds().toFloat();
            const float width = bounds.getWidth();
            const float height = bounds.getHeight();

            YGNodeCalculateLayout(yogaNode, width, height, YGDirectionInherit);

            // TODO: Maybe this shouldn't be View::resized() but should call its
            // own setBounds and iterate children. That way it doesn't clobber the
            // position assigned to it by its parent...?
            View::resized();
        }

        //==============================================================================
        /** Reads a JavaScript bundle from file and evaluates it in the Duktape context. */
        void runScript (const juce::File& f)
        {
            auto src = f.loadFileAsString();

            duk_push_string(ctx, src.toRawUTF8());

            if (duk_peval(ctx) != 0) {
                printf("Script evaluation failed: %s\n", duk_safe_to_string(ctx, -1));
            }

            duk_pop(ctx);
        }

        //==============================================================================
        /** Creates a new view instance and registers it with the view table.

            Returns the new view component id.
         */
        View* createViewInstance()
        {
            juce::Uuid id;
            juce::String sid = id.toDashedString();

            viewTable[sid] = std::make_unique<View>();
            viewTable[sid]->setComponentID(sid);

            return viewTable[sid].get();
        }

        View* getViewHandle (juce::String viewId)
        {
            if (viewId == getComponentID())
                return this;

            if (viewTable.find(viewId) != viewTable.end())
                return viewTable[viewId].get();

            // If we get here, you asked for a view that doesn't exist by that
            // identifier.
            jassertfalse;
        }

        /** Adds a child component to a given parent component. */
        void appendChild(juce::String parentId, juce::String childId)
        {
            jassert (parentId == getComponentID() || viewTable.find(parentId) != viewTable.end());
            jassert (viewTable.find(childId) != viewTable.end());

            if (parentId == getComponentID())
                return View::appendChild(viewTable[childId].get());

            viewTable[parentId]->appendChild(viewTable[childId].get());
        }

    private:
        //==============================================================================
        std::map<juce::String, std::unique_ptr<View>> viewTable;
        duk_context* ctx;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReactApplicationRoot)
    };

}
