/*
  ==============================================================================

    blueprint_ReactApplicationRoot.cpp
    Created: 9 Dec 2018 10:20:37am

  ==============================================================================
*/

#pragma once

#include "blueprint_ReactApplicationRoot.h"


namespace blueprint
{

    ReactApplicationRoot::ReactApplicationRoot(std::shared_ptr<EcmascriptEngine> ee)
        : viewManager(this)
        , engine(ee)
    {
        JUCE_ASSERT_MESSAGE_THREAD
        bindNativeRenderingHooks();
    }

    ReactApplicationRoot::ReactApplicationRoot()
        : ReactApplicationRoot(std::make_shared<EcmascriptEngine>())
    {

#if JUCE_DEBUG
        // Enable keyboardFocus to support CTRL-D/CMD-D debug attachment.
        setWantsKeyboardFocus(true);
#endif
    }

    //==============================================================================
    void ReactApplicationRoot::resized()
    {
        viewManager.performRootShadowTreeLayout();
    }

    void ReactApplicationRoot::paint(juce::Graphics& g)
    {
        if (errorText)
        {
            g.fillAll(juce::Colour(0xffe14c37));
            errorText->draw(g, getLocalBounds().toFloat().reduced(10.f));
        }
        else
        {
            View::paint(g);
        }
    }

#if JUCE_DEBUG
    bool ReactApplicationRoot::keyPressed(const juce::KeyPress& key)
    {
        const auto startDebugCommand = juce::KeyPress('d', juce::ModifierKeys::commandModifier, 0);

        if (key == startDebugCommand)
            engine->debuggerAttach();

        return true;
    }
#endif

    //==============================================================================
    juce::var ReactApplicationRoot::evaluate(const juce::File& bundle)
    {
        JUCE_ASSERT_MESSAGE_THREAD

        try
        {
            return engine->evaluate(bundle);
        }
        catch (const EcmascriptEngine::Error& err)
        {
            handleRuntimeError(err);
            return juce::var();
        }
    }

    //==============================================================================
    void ReactApplicationRoot::registerViewType(const juce::String& typeId, ViewManager::ViewFactory f)
    {
        viewManager.registerViewType(typeId, f);
    }

    //==============================================================================
    void ReactApplicationRoot::handleRuntimeError(const EcmascriptEngine::Error& err)
    {
#if ! JUCE_DEBUG
        // In release builds, we don't catch errors and show the red screen,
        // we allow the exception to raise up to the user to be handled properly
        // for a production app.
        throw err;
#endif

        JUCE_ASSERT_MESSAGE_THREAD

        DBG("");
        DBG("==== Error in JavaScript runtime. Context: ====");
        DBG(err.context);
        DBG("");
        DBG(err.what());

        errorText = std::make_unique<juce::AttributedString>(err.stack);

#if JUCE_WINDOWS
        errorText->setFont(juce::Font("Lucida Console", 18, juce::Font::FontStyleFlags::plain));
#elif JUCE_MAC
        errorText->setFont(juce::Font("Monaco", 18, juce::Font::FontStyleFlags::plain));
#else
        errorText->setFont(18);
#endif

        // Lastly, kill the ViewManager to tear down existing views and prevent
        // further view interaction
        viewManager.clearViewTables();

        repaint();
    }

    void ReactApplicationRoot::reset()
    {
        viewManager.clearViewTables();
        engine->reset();
        errorText = nullptr;
    }

    void ReactApplicationRoot::bindNativeRenderingHooks()
    {
        engine->registerNativeProperty("__BlueprintNative__", juce::JSON::parse("{}"));

        engine->registerNativeMethod("__BlueprintNative__", "createViewInstance", [this](const juce::var::NativeFunctionArgs& args) {
            jassert (args.numArguments == 1);

            auto viewType = args.arguments[0].toString();
            ViewId viewId = viewManager.createViewInstance(viewType);

            return juce::var(viewId);
        });

        engine->registerNativeMethod("__BlueprintNative__", "createTextViewInstance", [this](const juce::var::NativeFunctionArgs& args) {
            jassert (args.numArguments == 1);

            auto textValue = args.arguments[0].toString();
            auto viewId = viewManager.createTextViewInstance(textValue);

            return juce::var(viewId);
        });

        engine->registerNativeMethod("__BlueprintNative__", "setViewProperty", [this](const juce::var::NativeFunctionArgs& args) {
            jassert (args.numArguments == 3);

            ViewId viewId = args.arguments[0];
            auto propertyName = args.arguments[1].toString();
            auto propertyValue = args.arguments[2];

            viewManager.setViewProperty(viewId, propertyName, propertyValue);
            return juce::var::undefined();
        });

        engine->registerNativeMethod("__BlueprintNative__", "setRawTextValue", [this](const juce::var::NativeFunctionArgs& args) {
            jassert (args.numArguments == 2);

            ViewId viewId = args.arguments[0];
            auto textValue = args.arguments[1].toString();

            viewManager.setRawTextValue(viewId, textValue);
            return juce::var::undefined();
        });

        engine->registerNativeMethod("__BlueprintNative__", "addChild", [this](const juce::var::NativeFunctionArgs& args) {
            jassert (args.numArguments >= 2);

            ViewId parentId = args.arguments[0];
            ViewId childId = args.arguments[1];
            int index = -1;

            if (args.numArguments > 2)
                index = args.arguments[2];

            viewManager.addChild(parentId, childId, index);
            return juce::var::undefined();
        });

        engine->registerNativeMethod("__BlueprintNative__", "removeChild", [this](const juce::var::NativeFunctionArgs& args) {
            jassert (args.numArguments == 2);

            ViewId parentId = args.arguments[0];
            ViewId childId = args.arguments[1];

            viewManager.removeChild(parentId, childId);
            return juce::var::undefined();
        });

        engine->registerNativeMethod("__BlueprintNative__", "getRootInstanceId", [this](const juce::var::NativeFunctionArgs& args) {
            jassert (args.numArguments == 0);
            return juce::var(getViewId());
        });

        engine.registerNativeMethod("__BlueprintNative__", "resetAfterCommit", [this](const juce::var::NativeFunctionArgs& args) {
            // TODO, something else... traverse for dirty
            // yoga nodes
            getViewManager().performRootShadowTreeLayout();
            return juce::var::undefined();
        });
    }

}
