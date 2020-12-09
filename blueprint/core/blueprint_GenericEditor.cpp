namespace blueprint
{
    BlueprintGenericEditor::BlueprintGenericEditor (juce::AudioProcessor& proc, const juce::File& bundle, juce::AudioProcessorValueTreeState* vts) :
        juce::AudioProcessorEditor (proc),
        appRoot (engine),
        harness (appRoot),
        valueTreeState (vts)
    {
        // Sanity check
        jassert (bundle.existsAsFile());
        bundleFile = bundle;

        // Now we set up parameter listeners and register their current values.
        auto& params = processor.getParameters();
        paramReadouts.resize (params.size());

        for (auto* p : params)
        {
            auto& pr = paramReadouts.getReference (p->getParameterIndex());
            pr.value = p->getValue();
            pr.dirty = true;

            p->addListener (this);
        }

        // Set up the hot reloading callbacks
        harness.onBeforeAll = [this]() { beforeBundleEvaluated(); };
        harness.onAfterAll = [this]() { afterBundleEvaluated(); };

        // Set up the file watching and kick off the initial render
        harness.watch (bundleFile);
        harness.start();

        // Add ReactApplicationRoot as child component
        addAndMakeVisible (appRoot);

        // Set an arbitrary size, should be overridden from outside the constructor
        setSize (400, 200);

        // Lastly, start our timer for reporting meter and parameter values
        startTimerHz (30);
    }

    BlueprintGenericEditor::~BlueprintGenericEditor()
    {
        for (auto& p : processor.getParameters())
            p->removeListener (this);
    }

    //==============================================================================
    void BlueprintGenericEditor::parameterValueChanged (int parameterIndex, float newValue)
    {
        // This callback often runs on the realtime thread. To avoid any blocking
        // or non-deterministic operations, we simply set some atomic values in our
        // paramReadouts list. The timer running on the PluginEditor will check to
        // propagate the updated values to the javascript interface.
        auto& v = paramReadouts.getReference (parameterIndex);
        v.value = newValue;
        v.dirty = true;
    }

    void BlueprintGenericEditor::parameterGestureChanged (int parameterIndex, bool gestureIsStarting)
    {
        // Our generic editor doesn't do anything with this information yet, but
        // we'll happily take a pull request if you need something here :).
        juce::ignoreUnused (parameterIndex, gestureIsStarting);
    }

    //==============================================================================
    void BlueprintGenericEditor::timerCallback()
    {
        auto& params = processor.getParameters();
        jassert (params.size() == paramReadouts.size());

        for (int i = 0; i < paramReadouts.size(); ++i)
        {
            auto& pr = paramReadouts.getReference (i);

            if (pr.dirty)
            {
                const auto value = pr.value.load();
                pr.dirty = false;

                juce::String id, stringValue;
                float defaultValue = 0.0f;

                if (auto* p = params[i])
                {
                    id = p->getName (100);

                    if (auto* x = dynamic_cast<juce::AudioProcessorParameterWithID*>(p))
                        id = x->paramID;

                    defaultValue = p->getDefaultValue();
                    stringValue = p->getText(value, 0);
                }

                appRoot.dispatchEvent ("parameterValueChange",
                                       i,
                                       id,
                                       defaultValue,
                                       value,
                                       stringValue);
            }
        }
    }

    //==============================================================================
    void BlueprintGenericEditor::resized()
    {
        appRoot.setBounds (getLocalBounds());
    }

    void BlueprintGenericEditor::paint (juce::Graphics& g)
    {
        g.fillAll (juce::Colours::transparentWhite);
    }

    //==============================================================================
    void BlueprintGenericEditor::beforeBundleEvaluated()
    {
        if (valueTreeState == nullptr)
            return;

        engine->registerNativeMethod ("beginParameterChangeGesture",
            [this] (const juce::var::NativeFunctionArgs& args)
            {
                if (args.numArguments > 0)
                    if (auto* parameter = valueTreeState->getParameter (args.arguments[0].toString()))
                        parameter->beginChangeGesture();

                return juce::var::undefined();
            }
        );

        engine->registerNativeMethod ("setParameterValueNotifyingHost",
            [this] (const juce::var::NativeFunctionArgs& args)
            {
                if (args.numArguments > 0)
                    if (auto* parameter = valueTreeState->getParameter (args.arguments[0].toString()))
                        parameter->setValueNotifyingHost (args.arguments[1]);

                return juce::var::undefined();
            }
        );

        engine->registerNativeMethod ("endParameterChangeGesture",
            [this] (const juce::var::NativeFunctionArgs& args)
            {
                if (args.numArguments > 0)
                    if (auto* parameter = valueTreeState->getParameter (args.arguments[0].toString()))
                        parameter->endChangeGesture();

                return juce::var::undefined();
            }
        );
    }

    void BlueprintGenericEditor::afterBundleEvaluated()
    {
        for (auto& p : processor.getParameters())
            parameterValueChanged (p->getParameterIndex(), p->getValue());
    }
}
