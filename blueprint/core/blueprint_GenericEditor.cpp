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

        // Now we can provision the app root
        assignNewAppRoot(code);

        // If an error showed up, our appRoot is already gone
        if (appRoot)
            addAndMakeVisible(appRoot.get());

        // Set an arbitrary size, should be overriden from outside the constructor
        setSize(400, 200);
    }

    BlueprintGenericEditor::BlueprintGenericEditor (juce::AudioProcessor* processor, const juce::File& bundle, juce::AudioProcessorValueTreeState* vts)
        : juce::AudioProcessorEditor(processor), valueTreeState(vts)
    {
        // Sanity check
        jassert (bundle.existsAsFile());
        bundleFile = bundle;
        lastModifiedTime = bundleFile.getLastModificationTime();

        // Bind parameter listeners
        for (auto& p : processor->getParameters())
            p->addListener(this);

        // Now we can provision the app root
        assignNewAppRoot(bundle.loadFileAsString());

        // If an error showed up, our appRoot is already gone
        if (appRoot)
            addAndMakeVisible(appRoot.get());

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
        if (appRoot)
        {
            appRoot->dispatchEvent("parameterGestureChange", parameterIndex, gestureIsStarting);
        }
    }

    //==============================================================================
    void BlueprintGenericEditor::timerCallback()
    {
        auto lmt = bundleFile.getLastModificationTime();

        if (lmt > lastModifiedTime)
        {
            // Sanity check... again
            jassert (bundleFile.existsAsFile());

            // Remove and delete the current appRoot
            appRoot.reset();

            // Then we assign a new one
            assignNewAppRoot(bundleFile.loadFileAsString());

            // Add and set size, carefull
            if (appRoot)
            {
                addAndMakeVisible(appRoot.get());
                appRoot->setBounds(getLocalBounds());
            }

            lastModifiedTime = lmt;
        }
    }

    void BlueprintGenericEditor::resized()
    {
        // Evaluating the bundle in the app root might hit the error handler, which
        // deletes the appRoot, in turn alerting the parent component (this) to remove
        // its child and resize. So we're careful check the appRoot before doing anything
        if (appRoot)
        {
            appRoot->setBounds(getLocalBounds());
        }
    }

    void BlueprintGenericEditor::paint(juce::Graphics& g)
    {
        g.fillAll(juce::Colours::transparentWhite);

        if (errorText)
        {
            g.fillAll(juce::Colour(0xffe14c37));
            errorText->draw(g, getLocalBounds().toFloat().reduced(10.f));
        }
    }

    void BlueprintGenericEditor::assignNewAppRoot(const juce::String& code)
    {
        // Assign a fresh appRoot
        appRoot = std::make_unique<ReactApplicationRoot>();
        appRoot->engine.onUncaughtError = [this](const juce::String& msg, const juce::String& trace) {
            showError(trace);
        };

        // If we have a valueTreeState, bind parameter methods to the new app root
        if (valueTreeState != nullptr)
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
                (void *) valueTreeState
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
                (void *) valueTreeState
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
                (void *) valueTreeState
            );
        }

        // Now evaluate the code within the environment. We reset the error text ahead
        // of time, assuming the code will evaluate well
        errorText.reset();
        appRoot->evaluate(code);

        // At this point the appRoot may have been removed due to an error evaluating
        // the bundle, so we check for that case and halt if necessary
        if (!appRoot)
            return;

        // By now, things look good, let's push current parameter values into
        // the bundle
        for (auto& p : processor.getParameters())
            parameterValueChanged(p->getParameterIndex(), p->getValue());
    }

    void BlueprintGenericEditor::showError(const juce::String& trace)
    {
        appRoot.reset();
        errorText.reset(new juce::AttributedString(trace));
#if JUCE_WINDOWS
        errorText->setFont(juce::Font("Lucida Console", 18, juce::Font::FontStyleFlags::plain));
#elif JUCE_MAC
        errorText->setFont(juce::Font("Monaco", 18, juce::Font::FontStyleFlags::plain));
#else
        errorText->setFont(18);
#endif
        repaint();
    }

}
