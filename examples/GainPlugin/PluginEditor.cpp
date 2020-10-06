/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GainPluginAudioProcessorEditor::GainPluginAudioProcessorEditor (GainPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // First thing we have to do is load our javascript bundle from the build
    // directory so that we can evaluate it within our appRot.
    File sourceDir = File(__FILE__).getParentDirectory();
    File bundle = sourceDir.getChildFile("jsui/build/js/main.js");

    // Sanity check
    jassert (bundle.existsAsFile());

    // Bind some native callbacks
    appRoot.engine.registerNativeMethod(
        "beginParameterChangeGesture",
        [this](const juce::var::NativeFunctionArgs& args) {
            const juce::String& paramId = args.arguments[0].toString();

            if (auto* parameter = processor.getValueTreeState().getParameter(paramId))
                parameter->beginChangeGesture();

            return juce::var::undefined();
        }
    );

    appRoot.engine.registerNativeMethod(
        "setParameterValueNotifyingHost",
        [this](const juce::var::NativeFunctionArgs& args) {
            const juce::String& paramId = args.arguments[0].toString();
            const double value = args.arguments[1];

            if (auto* parameter = processor.getValueTreeState().getParameter(paramId))
                parameter->setValueNotifyingHost(value);

            return juce::var::undefined();
        }
    );

    appRoot.engine.registerNativeMethod(
        "endParameterChangeGesture",
        [this](const juce::var::NativeFunctionArgs& args) {
            const juce::String& paramId = args.arguments[0].toString();

            if (auto* parameter = processor.getValueTreeState().getParameter(paramId))
                parameter->endChangeGesture();

            return juce::var::undefined();
        }
    );

    // Next we just add our appRoot and kick off the app bundle.
    addAndMakeVisible(appRoot);
    appRoot.evaluate(bundle);

    // Now we set up parameter listeners and register their current values.
    // For this example plugin, we only have the one "Gain" parameter, but
    // this example serves to show an approach that scales to arbitrary numbers
    // of parameters.
    auto& params = processor.getParameters();
    paramReadouts.resize(params.size());

    for (auto& p : params)
    {
        const auto index = p->getParameterIndex();
        const auto value = p->getValue();

        paramReadouts[index].value = value;
        paramReadouts[index].dirty = true;

        p->addListener(this);
    }

    // Lastly, start our timer for reporting meter and parameter values
    startTimerHz(30);

    // And of course set our editor size before we're done.
    setResizable(true, true);
    setResizeLimits(400, 240, 400 * 2, 240 * 2);
    getConstrainer()->setFixedAspectRatio(400.0 / 240.0);
    setSize (400, 240);
}

GainPluginAudioProcessorEditor::~GainPluginAudioProcessorEditor()
{
    stopTimer();

    // Tear down parameter listeners
    for (auto& p : processor.getParameters())
        p->removeListener(this);
}

//==============================================================================
void GainPluginAudioProcessorEditor::paint (Graphics& g)
{
    // We'll do all of our drawing via the child components assembled
    // under the appROot.
}

void GainPluginAudioProcessorEditor::resized()
{
    // For this example we'll build the whole UI in javascript, so just
    // let the appRoot take over the whole editor area.
    appRoot.setBounds(getLocalBounds());
}

//==============================================================================
void GainPluginAudioProcessorEditor::parameterValueChanged (int parameterIndex, float newValue)
{
    // This callback often runs on the realtime thread. To avoid any blocking
    // or non-deterministic operations, we simply set some atomic values in our
    // paramReadouts list. The timer running on the PluginEditor will check to
    // propagate the updated values to the javascript interface.
    paramReadouts[parameterIndex].value = newValue;
    paramReadouts[parameterIndex].dirty = true;
}

void GainPluginAudioProcessorEditor::timerCallback()
{
    // Dispatch the gain peak values
    appRoot.dispatchEvent(
        "gainPeakValues",
        processor.getLeftChannelPeak(),
        processor.getRightChannelPeak()
    );

    // Then we iterate here to dispatch any updated parameter values
    for (int i = 0; i < paramReadouts.size(); ++i)
    {
        auto& pr = paramReadouts[i];

        if (pr.dirty)
        {
            const float value = pr.value.load();
            pr.dirty = false;

            const auto& p = processor.getParameters()[i];
            juce::String id = p->getName(100);

            if (auto* x = dynamic_cast<AudioProcessorParameterWithID*>(p))
                id = x->paramID;

            float defaultValue = p->getDefaultValue();
            juce::String stringValue = p->getText(value, 0);

            appRoot.dispatchEvent("parameterValueChange",
                                  i,
                                  id,
                                  defaultValue,
                                  value,
                                  stringValue);
        }
    }
}
