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
        [](void* stash, const juce::var::NativeFunctionArgs& args) {
            auto* self = reinterpret_cast<GainPluginAudioProcessorEditor*>(stash);
            const juce::String& paramId = args.arguments[0].toString();

            if (auto* parameter = self->processor.getValueTreeState().getParameter(paramId))
                parameter->beginChangeGesture();

            return juce::var::undefined();
        },
        (void *) this
    );

    appRoot.engine.registerNativeMethod(
        "setParameterValueNotifyingHost",
        [](void* stash, const juce::var::NativeFunctionArgs& args) {
            auto* self = reinterpret_cast<GainPluginAudioProcessorEditor*>(stash);
            const juce::String& paramId = args.arguments[0].toString();
            const double value = args.arguments[1];

            if (auto* parameter = self->processor.getValueTreeState().getParameter(paramId))
                parameter->setValueNotifyingHost(value);

            return juce::var::undefined();
        },
        (void *) this
    );

    appRoot.engine.registerNativeMethod(
        "endParameterChangeGesture",
        [](void* stash, const juce::var::NativeFunctionArgs& args) {
            auto* self = reinterpret_cast<GainPluginAudioProcessorEditor*>(stash);
            const juce::String& paramId = args.arguments[0].toString();

            if (auto* parameter = self->processor.getValueTreeState().getParameter(paramId))
                parameter->endChangeGesture();

            return juce::var::undefined();
        },
        (void *) this
    );

    // Next we just add our appRoot and kick off the app bundle.
    addAndMakeVisible(appRoot);
    appRoot.evaluate(bundle.loadFileAsString());

    // Now our React application is up and running, so we can start dispatching
    // events, such as current parameter values.
    for (auto& p : processor.getParameters())
    {
        p->addListener(this);
        p->sendValueChangedMessageToListeners(p->getValue());
    }

    // Lastly, start our timer for reporting meter values
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
    const auto& p = processor.getParameters()[parameterIndex];
    juce::String id = p->getName(100);

    if (auto* x = dynamic_cast<AudioProcessorParameterWithID*>(p))
        id = x->paramID;

    float defaultValue = p->getDefaultValue();
    juce::String stringValue = p->getText(newValue, 0);

    Component::SafePointer<blueprint::ReactApplicationRoot> safeAppRoot (&appRoot);

    juce::MessageManager::callAsync([=]() {
        if (blueprint::ReactApplicationRoot* root = safeAppRoot.getComponent())
            root->dispatchEvent("parameterValueChange",
                                parameterIndex,
                                id,
                                defaultValue,
                                newValue,
                                stringValue);
    });
}

void GainPluginAudioProcessorEditor::timerCallback()
{
    appRoot.dispatchEvent(
        "gainPeakValues",
        processor.getLeftChannelPeak(),
        processor.getRightChannelPeak()
    );
}
