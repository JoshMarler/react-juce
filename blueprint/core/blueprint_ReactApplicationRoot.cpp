/*
  ==============================================================================

    blueprint_ReactApplicationRoot.cpp
    Created: 9 Dec 2018 10:20:37am

  ==============================================================================
*/

#pragma once


namespace blueprint
{

    ReactApplicationRoot* ReactApplicationRoot::singletonInstance = nullptr;

    duk_ret_t BlueprintNative::createViewInstance (duk_context *ctx)
    {
        jassert (ReactApplicationRoot::singletonInstance != nullptr);

        View* view = ReactApplicationRoot::singletonInstance->createViewInstance();
        duk_push_string(ctx, view->getComponentID().toRawUTF8());

        return 1;
    };

    duk_ret_t BlueprintNative::createTextViewInstance (duk_context *ctx)
    {
        jassert (ReactApplicationRoot::singletonInstance != nullptr);
        jassert (duk_is_string(ctx, 0));

        ReactApplicationRoot* root = ReactApplicationRoot::singletonInstance;
        juce::String textValue = duk_get_string(ctx, 0);
        View* view = root->createTextViewInstance(textValue);

        duk_push_string(ctx, view->getComponentID().toRawUTF8());
        return 1;
    };

    duk_ret_t BlueprintNative::setViewProperty (duk_context *ctx)
    {
        jassert (ReactApplicationRoot::singletonInstance != nullptr);
        jassert (duk_is_string(ctx, 0) && duk_is_string(ctx, 1));
        jassert (duk_get_type_mask(ctx, 2) & (DUK_TYPE_MASK_NUMBER | DUK_TYPE_MASK_STRING));

        ReactApplicationRoot* root = ReactApplicationRoot::singletonInstance;

        juce::String instanceId = duk_get_string(ctx, 0);
        juce::String propertyName = duk_get_string(ctx, 1);
        juce::var propertyValue = (duk_is_string(ctx, 2)
                                   ? juce::var (duk_get_string(ctx, 2))
                                   : juce::var (duk_get_number(ctx, 2)));

        View* view = root->getViewHandle(instanceId);
        view->setProperty(propertyName, propertyValue);

        return 0;
    };

    duk_ret_t BlueprintNative::appendChild (duk_context *ctx)
    {
        jassert (ReactApplicationRoot::singletonInstance != nullptr);

        ReactApplicationRoot* root = ReactApplicationRoot::singletonInstance;

        juce::String parentId = duk_get_string(ctx, 0);
        juce::String childId = duk_get_string(ctx, 1);

        View* parentView = root->getViewHandle(parentId);
        View* childView = root->getViewHandle(childId);

        parentView->appendChild(childView);
        return 0;
    };

    duk_ret_t BlueprintNative::getRootInstanceId (duk_context *ctx)
    {
        jassert (ReactApplicationRoot::singletonInstance != nullptr);

        ReactApplicationRoot* root = ReactApplicationRoot::singletonInstance;
        duk_push_string(ctx, root->getComponentID().toRawUTF8());

        return 1;
    }

    duk_context* initializeDuktapeContext()
    {
        // Allocate a new js heap
        duk_context* ctx = duk_create_heap_default();

        // Add console.log support
        duk_console_init(ctx, DUK_CONSOLE_FLUSH);

        // Register react render backend functions
        const duk_function_list_entry blueprintNativeFuncs[] = {
            { "createViewInstance", BlueprintNative::createViewInstance, 1},
            { "createTextViewInstance", BlueprintNative::createTextViewInstance, 1},
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

        return ctx;
    }

}
