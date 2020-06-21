/*
  ==============================================================================

    blueprint_GenericEditor.cpp
    Created: 3 Nov 2019 4:47:39pm

  ==============================================================================
*/

#pragma once


namespace blueprint
{

    //==============================================================================
    BlueprintGenericEditor::BlueprintGenericEditor (juce::AudioProcessor* processor, const juce::String& code, juce::AudioProcessorValueTreeState* vts)
        : juce::AudioProcessorEditor(processor), valueTreeState(vts)
    {
        // Bind parameter listeners
        for (auto& p : processor->getParameters())
            p->addListener(this);

        // Setup the ReactApplicationRoot callbacks and evaluate the supplied JS code/bundle
        registerAppRootCallbacks();
        appRoot.evaluate(code);

        // Add ReactApplicationRoot as child component
        addAndMakeVisible(appRoot);

        // Set an arbitrary size, should be overridden from outside the constructor
        setSize(400, 200);
    }

    BlueprintGenericEditor::BlueprintGenericEditor (juce::AudioProcessor* processor, const juce::File& bundle, juce::AudioProcessorValueTreeState* vts)
        : juce::AudioProcessorEditor(processor), valueTreeState(vts)
    {
        // Sanity check
        jassert (bundle.existsAsFile());
        bundleFile = bundle;

        // Bind parameter listeners
        for (auto& p : processor->getParameters())
            p->addListener(this);

        // Setup the ReactApplicationRoot callbacks and evaluate the supplied JS code/bundle
        registerAppRootCallbacks();
        appRoot.evaluate(bundleFile);

        // Add ReactApplicationRoot as child component
        addAndMakeVisible(appRoot);

        // Set an arbitrary size, should be overridden from outside the constructor
        setSize(400, 200);
    }

    BlueprintGenericEditor::~BlueprintGenericEditor()
    {
        for (auto& p : processor.getParameters())
        {
            p->removeListener(this);
        }
    }

    //==============================================================================
    void BlueprintGenericEditor::parameterValueChanged (int parameterIndex, float newValue)
    {
        // Collect some information about the parameter to push into the engine
        const auto& p = processor.getParameters()[parameterIndex];
        juce::String id = p->getName(100);

        if (auto* x = dynamic_cast<juce::AudioProcessorParameterWithID*>(p))
            id = x->paramID;

        float defaultValue = p->getDefaultValue();
        const juce::String stringValue = p->getText(newValue, 0);

        // Dispatch parameter value updates to the javascript engine at 30Hz
        throttleMap.throttle(parameterIndex, 1000.0 / 30.0, [=]() mutable {
            juce::MessageManager::callAsync([=]() mutable {
                appRoot.dispatchEvent(
                    "parameterValueChange",
                    parameterIndex,
                    id,
                    defaultValue,
                    newValue,
                    stringValue
                );
            });
        });
    }

    void BlueprintGenericEditor::parameterGestureChanged (int parameterIndex, bool gestureIsStarting)
    {
        // We don't need to worry so much about throttling gesture events since they happen far
        // more slowly than value changes
        appRoot.dispatchEvent("parameterGestureChange", parameterIndex, gestureIsStarting);
    }

    //==============================================================================
    void BlueprintGenericEditor::resized()
    {
        // Ensure our ReactApplicationRoot always fills the entire bounds
        // of this editor.
        appRoot.setBounds(getLocalBounds());
    }

    void BlueprintGenericEditor::paint(juce::Graphics& g)
    {
        g.fillAll(juce::Colours::transparentWhite);
    }

    //==============================================================================
    void BlueprintGenericEditor::beforeBundleEvaluated()
    {
        // If we have a valueTreeState, bind parameter methods to the new app root
        if (valueTreeState != nullptr)
        {
            appRoot.engine.registerNativeMethod(
                    "beginParameterChangeGesture",
                    [](void* stash, const juce::var::NativeFunctionArgs& args) {
                        auto* state = reinterpret_cast<juce::AudioProcessorValueTreeState*>(stash);
                        const juce::String& paramId = args.arguments[0].toString();

                        if (auto* parameter = state->getParameter(paramId))
                            parameter->beginChangeGesture();

                        return juce::var::undefined();
                    },
                    (void *) valueTreeState
            );

            appRoot.engine.registerNativeMethod(
                    "setParameterValueNotifyingHost",
                    [](void* stash, const juce::var::NativeFunctionArgs& args) {
                        auto* state = reinterpret_cast<juce::AudioProcessorValueTreeState*>(stash);
                        const juce::String& paramId = args.arguments[0].toString();
                        const double value = args.arguments[1];

                        if (auto* parameter = state->getParameter(paramId))
                            parameter->setValueNotifyingHost(value);

                        return juce::var::undefined();
                    },
                    (void *) valueTreeState
            );

            appRoot.engine.registerNativeMethod(
                    "endParameterChangeGesture",
                    [](void* stash, const juce::var::NativeFunctionArgs& args) {
                        auto* state = reinterpret_cast<juce::AudioProcessorValueTreeState*>(stash);
                        const juce::String& paramId = args.arguments[0].toString();

                        if (auto* parameter = state->getParameter(paramId))
                            parameter->endChangeGesture();

                        return juce::var::undefined();
                    },
                    (void *) valueTreeState
            );
        }
    }

    void BlueprintGenericEditor::afterBundleEvaluated()
    {
        // Push current parameter values into the bundle on load
        for (auto& p : processor.getParameters())
            parameterValueChanged(p->getParameterIndex(), p->getValue());
    }

    void BlueprintGenericEditor::registerAppRootCallbacks()
    {
        appRoot.beforeBundleEval = [=] (std::optional<juce::File> bundle)
        {
            if (bundle && bundle->getFullPathName() == bundleFile.getFullPathName())
            {
                beforeBundleEvaluated();
            }
        };

        appRoot.afterBundleEval = [=] (std::optional<juce::File> bundle)
        {
            if (bundle && bundle->getFullPathName() == bundleFile.getFullPathName())
            {
                afterBundleEvaluated();
            }
        };
    }
}
