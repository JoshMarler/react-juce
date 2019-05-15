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
        jassert (duk_is_string(ctx, 0));

        ReactApplicationRoot* root = ReactApplicationRoot::singletonInstance;
        juce::String viewType = duk_get_string(ctx, 0);
        ViewId viewId = root->createViewInstance(viewType);

        duk_push_int(ctx, viewId);
        return 1;
    };

    duk_ret_t BlueprintNative::createTextViewInstance (duk_context *ctx)
    {
        jassert (ReactApplicationRoot::singletonInstance != nullptr);
        jassert (duk_is_string(ctx, 0));

        ReactApplicationRoot* root = ReactApplicationRoot::singletonInstance;
        juce::String textValue = duk_get_string(ctx, 0);
        ViewId viewId = root->createTextViewInstance(textValue);

        duk_push_int(ctx, viewId);
        return 1;
    };

    duk_ret_t BlueprintNative::setViewProperty (duk_context *ctx)
    {
        jassert (ReactApplicationRoot::singletonInstance != nullptr);
        jassert (duk_is_number(ctx, 0) && duk_is_string(ctx, 1));

        ReactApplicationRoot* root = ReactApplicationRoot::singletonInstance;
        ViewId viewId = duk_get_number(ctx, 0);
        juce::String propertyName = duk_get_string(ctx, 1);
        juce::var propertyValue;

        switch (duk_get_type(ctx, 2))
        {
            case DUK_TYPE_STRING:
                propertyValue = duk_get_string(ctx, 2);
                break;
            case DUK_TYPE_NUMBER:
                propertyValue = duk_get_number(ctx, 2);
                break;
            case DUK_TYPE_BOOLEAN:
                propertyValue = (bool) duk_get_boolean(ctx, 2);
                break;
            default:
                jassertfalse;
        }

        root->setViewProperty(viewId, propertyName, propertyValue);
        return 0;
    };

    duk_ret_t BlueprintNative::setRawTextValue (duk_context *ctx)
    {
        jassert (ReactApplicationRoot::singletonInstance != nullptr);
        jassert (duk_is_number(ctx, 0) && duk_is_string(ctx, 1));

        ReactApplicationRoot* root = ReactApplicationRoot::singletonInstance;
        ViewId viewId = duk_get_number(ctx, 0);
        juce::String value = duk_get_string(ctx, 1);

        root->setRawTextValue(viewId, value);
        return 0;
    };

    duk_ret_t BlueprintNative::appendChild (duk_context *ctx)
    {
        jassert (ReactApplicationRoot::singletonInstance != nullptr);
        jassert (duk_is_number(ctx, 0) && duk_is_number(ctx, 1));

        ReactApplicationRoot* root = ReactApplicationRoot::singletonInstance;
        ViewId parentId = duk_get_number(ctx, 0);
        ViewId childId = duk_get_number(ctx, 1);

        root->appendChild(parentId, childId);
        return 0;
    };

    duk_ret_t BlueprintNative::getRootInstanceId (duk_context *ctx)
    {
        jassert (ReactApplicationRoot::singletonInstance != nullptr);

        ReactApplicationRoot* root = ReactApplicationRoot::singletonInstance;
        duk_push_int(ctx, root->getViewId());

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
            { "setRawTextValue", BlueprintNative::setRawTextValue, 2},
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
