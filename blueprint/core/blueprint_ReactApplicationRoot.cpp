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

    duk_ret_t blueprint_create_instance (duk_context *ctx)
    {
        DBG("NativeFunction called from JavaScript environment!");
        return 0;
    };

}
