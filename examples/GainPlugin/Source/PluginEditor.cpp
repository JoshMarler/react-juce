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
    appRoot.registerNativeMethod(
        "beginParameterChangeGesture",
        [this](const juce::var::NativeFunctionArgs& args) {
            const juce::String& paramId = args.arguments[0].toString();

            if (auto* parameter = processor.getValueTreeState().getParameter(paramId))
                parameter->beginChangeGesture();
        }
    );

    appRoot.registerNativeMethod(
        "setParameterValueNotifyingHost",
        [this](const juce::var::NativeFunctionArgs& args) {
            const juce::String& paramId = args.arguments[0].toString();
            const double value = args.arguments[1];

            if (auto* parameter = processor.getValueTreeState().getParameter(paramId))
                parameter->setValueNotifyingHost(value);
        }
    );

    appRoot.registerNativeMethod(
        "endParameterChangeGesture",
        [this](const juce::var::NativeFunctionArgs& args) {
            const juce::String& paramId = args.arguments[0].toString();

            if (auto* parameter = processor.getValueTreeState().getParameter(paramId))
                parameter->endChangeGesture();
        }
    );

    // Next we just add our appRoot and kick off the app bundle.
    addAndMakeVisible(appRoot);
    appRoot.evalScript(bundle.loadFileAsString());

    // Now our React application is up and running, so we can start dispatching
    // events, such as current parameter values.
    for (auto& p : processor.getParameters())
    {
        p->addListener(this);
        p->sendValueChangedMessageToListeners(p->getValue());
    }

    // And of course set our editor size before we're done.
    setResizable(true, true);
    setResizeLimits(400, 240, 400 * 2, 240 * 2);
    getConstrainer()->setFixedAspectRatio(400.0 / 240.0);
    setSize (400, 240);
}

GainPluginAudioProcessorEditor::~GainPluginAudioProcessorEditor()
{
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
