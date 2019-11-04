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
        appRoot = std::make_unique<ReactApplicationRoot>();

        if (valueTreeState != nullptr)
            registerParameterMethods(valueTreeState);

        // Add our appRoot and kick off the app bundle; no file management to
        // worry about when given just a string.
        addAndMakeVisible(appRoot.get());
        appRoot->evaluate(code);

        // Now our React application is up and running, so we can start dispatching
        // events, such as current parameter values. Here we add the listeners
        // then dispatch the initial parameter value immediately
        for (auto& p : processor->getParameters())
        {
            p->addListener(this);
            parameterValueChanged(p->getParameterIndex(), p->getValue());
        }

        // Set an arbitrary size, should be overriden from outside the constructor
        setSize(400, 200);
    }

    BlueprintGenericEditor::BlueprintGenericEditor (juce::AudioProcessor* processor, const juce::File& bundle, juce::AudioProcessorValueTreeState* vts)
        : juce::AudioProcessorEditor(processor), valueTreeState(vts)
    {
        appRoot = std::make_unique<ReactApplicationRoot>();

        // Sanity check
        jassert (bundle.existsAsFile());
        bundleFile = bundle;
        lastModifiedTime = bundleFile.getLastModificationTime();

        if (valueTreeState != nullptr)
            registerParameterMethods(valueTreeState);

        // Next we just add our appRoot and kick off the app bundle.
        addAndMakeVisible(appRoot.get());
        appRoot->evaluate(bundle.loadFileAsString());

        // Now our React application is up and running, so we can start dispatching
        // events, such as current parameter values. Here we add the listeners
        // then dispatch the initial parameter value immediately
        for (auto& p : processor->getParameters())
        {
            p->addListener(this);
            parameterValueChanged(p->getParameterIndex(), p->getValue());
        }

        // Kick off the timer that polls for file changes
        startTimer(50);

        // Set an arbitrary size, should be overriden from outside the constructor
        setSize(400, 200);
    }

    BlueprintGenericEditor::~BlueprintGenericEditor()
    {
        stopTimer();

        for (auto& p : processor.getParameters())
        {
            p->removeListener(this);
        }
    }

    //==============================================================================
    void BlueprintGenericEditor::parameterValueChanged (int parameterIndex, float newValue)
    {
        Component::SafePointer<blueprint::ReactApplicationRoot> safeAppRoot (appRoot.get());

        // Collect some information about the parameter to push into the engine
        const auto& p = processor.getParameters()[parameterIndex];
        juce::String id = p->getName(100);

        if (auto* x = dynamic_cast<juce::AudioProcessorParameterWithID*>(p))
            id = x->paramID;

        float defaultValue = p->getDefaultValue();
        const juce::String stringValue = p->getText(newValue, 0);

        // Dispatch parameter value updates to the javascript engine at 30Hz
        throttleMap.throttle(parameterIndex, 1000.0 / 30.0, [=]() {
            juce::MessageManager::callAsync([=]() mutable {
                if (safeAppRoot)
                {
                    safeAppRoot->dispatchEvent(
                        "parameterValueChange",
                        parameterIndex,
                        id,
                        defaultValue,
                        newValue,
                        stringValue
                    );
                }
            });
        });
    }

    void BlueprintGenericEditor::parameterGestureChanged (int parameterIndex, bool gestureIsStarting)
    {
        // We don't need to worry so much about throttling gesture events since they happen far
        // more slowly than value changes
        appRoot->dispatchEvent("parameterGestureChange", parameterIndex, gestureIsStarting);
    }

    //==============================================================================
    void BlueprintGenericEditor::timerCallback()
    {
        auto lmt = bundleFile.getLastModificationTime();

        if (lmt > lastModifiedTime)
        {
            // Pop the root and its subtree out of the component heirarchy
            removeChildComponent(appRoot.get());

            // Then swap in a new ReactApplicationRoot, destroying the previous one
            appRoot = std::make_unique<ReactApplicationRoot>();

            // Sanity check... again
            jassert (bundleFile.existsAsFile());

            if (valueTreeState != nullptr)
                registerParameterMethods(valueTreeState);

            // Now we kick off the new bundle
            addAndMakeVisible(appRoot.get());
            appRoot->evaluate(bundleFile.loadFileAsString());
            appRoot->setBounds(getLocalBounds());

            // And dispatch current parameter values
            for (auto& p : processor.getParameters())
                parameterValueChanged(p->getParameterIndex(), p->getValue());

            lastModifiedTime = lmt;
        }
    }

    void BlueprintGenericEditor::registerParameterMethods(const juce::AudioProcessorValueTreeState* vts)
    {
        appRoot->engine.registerNativeMethod(
            "beginParameterChangeGesture",
            [](void* stash, const juce::var::NativeFunctionArgs& args) {
                auto* state = reinterpret_cast<juce::AudioProcessorValueTreeState*>(stash);
                const juce::String& paramId = args.arguments[0].toString();

                if (auto* parameter = state->getParameter(paramId))
                    parameter->beginChangeGesture();

                return juce::var::undefined();
            },
            (void *) vts
        );

        appRoot->engine.registerNativeMethod(
            "setParameterValueNotifyingHost",
            [](void* stash, const juce::var::NativeFunctionArgs& args) {
                auto* state = reinterpret_cast<juce::AudioProcessorValueTreeState*>(stash);
                const juce::String& paramId = args.arguments[0].toString();
                const double value = args.arguments[1];

                if (auto* parameter = state->getParameter(paramId))
                    parameter->setValueNotifyingHost(value);

                return juce::var::undefined();
            },
            (void *) vts
        );

        appRoot->engine.registerNativeMethod(
            "endParameterChangeGesture",
            [](void* stash, const juce::var::NativeFunctionArgs& args) {
                auto* state = reinterpret_cast<juce::AudioProcessorValueTreeState*>(stash);
                const juce::String& paramId = args.arguments[0].toString();

                if (auto* parameter = state->getParameter(paramId))
                    parameter->endChangeGesture();

                return juce::var::undefined();
            },
            (void *) vts
        );
    }

    void BlueprintGenericEditor::resized()
    {
        appRoot->setBounds(getLocalBounds());
    }

}
