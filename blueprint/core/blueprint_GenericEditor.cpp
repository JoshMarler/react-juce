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
    BlueprintGenericEditor::BlueprintGenericEditor (juce::AudioProcessor& proc, const juce::File& bundle, juce::AudioProcessorValueTreeState* vts)
        : juce::AudioProcessorEditor (proc), valueTreeState(vts)
    {
        // Sanity check
        jassert (bundle.existsAsFile());
        bundleFile = bundle;

        // Bind parameter listeners
        for (auto& p : proc.getParameters())
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

                        if (auto* parameter = state->getParameter (args.arguments[0].toString()))
                            parameter->beginChangeGesture();

                        return juce::var::undefined();
                    },
                    (void *) valueTreeState
            );

            appRoot.engine.registerNativeMethod(
                    "setParameterValueNotifyingHost",
                    [](void* stash, const juce::var::NativeFunctionArgs& args) {
                        auto* state = reinterpret_cast<juce::AudioProcessorValueTreeState*>(stash);

                        if (auto* parameter = state->getParameter (args.arguments[0].toString()))
                            parameter->setValueNotifyingHost (static_cast<float> (args.arguments[1]));

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
        appRoot.beforeBundleEval = [=] (const juce::File& bundle)
        {
            if (bundle.getFullPathName() == bundleFile.getFullPathName())
            {
                beforeBundleEvaluated();
            }
        };

        appRoot.afterBundleEval = [=] (const juce::File& bundle)
        {
            if (bundle.getFullPathName() == bundleFile.getFullPathName())
            {
                afterBundleEvaluated();
            }
        };
    }
}
