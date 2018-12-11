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

        juce::String sid = ReactApplicationRoot::singletonInstance->createViewInstance();

        duk_push_string(ctx, sid.toRawUTF8());
        return 1;
    };

    duk_ret_t BlueprintNative::setViewProperty (duk_context *ctx)
    {
        jassert (ReactApplicationRoot::singletonInstance != nullptr);
        jassert (duk_is_string(ctx, 0) && duk_is_string(ctx, 1));
        jassert (duk_get_type_mask(ctx, 2) & (DUK_TYPE_MASK_NUMBER | DUK_TYPE_MASK_STRING));

        juce::String parentId = duk_get_string(ctx, 0);
        juce::String propertyName = duk_get_string(ctx, 1);
        juce::var propertyValue = (duk_is_string(ctx, 2)
                                   ? juce::var (duk_get_string(ctx, 2))
                                   : juce::var (duk_get_number(ctx, 2)));

        // ReactApplicationRoot::singletonInstance->setProperty()
        return 0;
    };

    duk_ret_t BlueprintNative::appendChild (duk_context *ctx)
    {
        jassert (ReactApplicationRoot::singletonInstance != nullptr);

        juce::String parentId = duk_get_string(ctx, 0);
        juce::String childId = duk_get_string(ctx, 1);

        ReactApplicationRoot::singletonInstance->appendChild(parentId, childId);
        return 0;
    };

    duk_ret_t BlueprintNative::getRootInstance (duk_context *ctx)
    {
        jassert (ReactApplicationRoot::singletonInstance != nullptr);

        juce::String sid = ReactApplicationRoot::singletonInstance->getRootInstance();
        duk_push_string(ctx, sid.toRawUTF8());

        return 1;
    }

}
