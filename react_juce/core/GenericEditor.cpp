/*
  ==============================================================================

    GenericEditor.cpp
    Created: 3 Nov 2019 4:47:39pm

  ==============================================================================
*/

#include "GenericEditor.h"


namespace reactjuce
{

    //==============================================================================
    GenericEditor::GenericEditor (juce::AudioProcessor& proc, const juce::File& bundle)
        : juce::AudioProcessorEditor (proc)
        , engine(std::make_shared<EcmascriptEngine>())
        , appRoot(engine)
        , harness(appRoot)
    {
        // Sanity check
        jassert (bundle.existsAsFile());
        bundleFile = bundle;

        // Now we set up parameter listeners and register their current values.
        auto& params = processor.getParameters();
        paramReadouts.resize(static_cast<size_t>(params.size()));

        for (auto& p : params)
        {
            // Store the parameter ID for easy lookup in gesture lambdas
            if (auto paramWithID = dynamic_cast<juce::AudioProcessorParameterWithID*>(p)) {
                parameters.emplace(paramWithID->paramID, p);
            }

            const auto index = static_cast<size_t>(p->getParameterIndex());
            const auto value = p->getValue();

            paramReadouts[index].value = value;
            paramReadouts[index].dirty = true;

            p->addListener(this);
        }

        // Set up the hot reloading callbacks
        harness.onBeforeAll = [this]() { beforeBundleEvaluated(); };
        harness.onAfterAll = [this]() { afterBundleEvaluated(); };

        // Set up the file watching and kick off the initial render
        harness.watch(bundleFile);
        harness.start();

        // Add ReactApplicationRoot as child component
        addAndMakeVisible(appRoot);

        // Set an arbitrary size, should be overridden from outside the constructor
        setSize(400, 200);

        // Lastly, start our timer for reporting meter and parameter values
        startTimerHz(30);
    }

    GenericEditor::~GenericEditor()
    {
        for (auto& p : processor.getParameters())
        {
            p->removeListener(this);
        }
    }

    //==============================================================================
    void GenericEditor::parameterValueChanged (int parameterIndex, float newValue)
    {
        // This callback often runs on the realtime thread. To avoid any blocking
        // or non-deterministic operations, we simply set some atomic values in our
        // paramReadouts list. The timer running on the PluginEditor will check to
        // propagate the updated values to the javascript interface.
        paramReadouts[static_cast<size_t>(parameterIndex)].value = newValue;
        paramReadouts[static_cast<size_t>(parameterIndex)].dirty = true;
    }

    void GenericEditor::parameterGestureChanged (int, bool)
    {
        // Our generic editor doesn't do anything with this information yet, but
        // we'll happily take a pull request if you need something here :).
    }

    //==============================================================================
    void GenericEditor::timerCallback()
    {
        // Iterate here to dispatch any updated parameter values
        for (size_t i = 0; i < paramReadouts.size(); ++i)
        {
            auto& pr = paramReadouts[i];

            if (pr.dirty)
            {
                const float value = pr.value.load();
                pr.dirty = false;

                const auto& p = processor.getParameters()[(int) i];
                juce::String id = p->getName(100);

                if (auto* x = dynamic_cast<juce::AudioProcessorParameterWithID*>(p))
                    id = x->paramID;

                float defaultValue = p->getDefaultValue();
                juce::String stringValue = p->getText(value, 0);

                appRoot.dispatchEvent(
                    "parameterValueChange",
                    static_cast<int>(i),
                    id,
                    defaultValue,
                    value,
                    stringValue
                );
            }
        }
    }

    //==============================================================================
    void GenericEditor::resized()
    {
        // Ensure our ReactApplicationRoot always fills the entire bounds
        // of this editor.
        appRoot.setBounds(getLocalBounds());
    }

    void GenericEditor::paint(juce::Graphics& g)
    {
        g.fillAll(juce::Colours::transparentWhite);
    }

    //==============================================================================
    void GenericEditor::beforeBundleEvaluated()
    {
        engine->registerNativeMethod(
            "beginParameterChangeGesture",
            [this](const juce::var::NativeFunctionArgs& args) {
                if (auto it = parameters.find (args.arguments[0].toString()); it != parameters.cend())
                    it->second->beginChangeGesture();

                return juce::var::undefined();
            }
        );

        engine->registerNativeMethod(
            "setParameterValueNotifyingHost",
            [this](const juce::var::NativeFunctionArgs& args) {
                if (auto it = parameters.find (args.arguments[0].toString()); it != parameters.cend())
                    it->second->setValueNotifyingHost(args.arguments[1]);

                return juce::var::undefined();
            }
        );

        engine->registerNativeMethod(
            "endParameterChangeGesture",
            [this](const juce::var::NativeFunctionArgs& args) {
                if (auto it = parameters.find (args.arguments[0].toString()); it != parameters.cend())
                    it->second->endChangeGesture();

                return juce::var::undefined();
            }
        );
    }

    void GenericEditor::afterBundleEvaluated()
    {
        // Push current parameter values into the bundle on load
        for (auto& p : processor.getParameters())
            parameterValueChanged(p->getParameterIndex(), p->getValue());
    }

}
