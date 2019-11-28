/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BlueprintPluginAudioProcessorEditor::BlueprintPluginAudioProcessorEditor (BlueprintPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    File sourceDir = (File (__FILE__)).getParentDirectory();

    addAndMakeVisible(appRoot);
    appRoot.evaluate(sourceDir.getChildFile("ui/build/js/main.js").loadFileAsString());
    //appRoot.enableHotkeyReloading();
    
    appRoot.engine.registerNativeMethod(
        "setParameterValueNotifyingHost",
        [](void* stash, const juce::var::NativeFunctionArgs& args) {
            auto* self = reinterpret_cast<BlueprintPluginAudioProcessorEditor*>(stash);
            const juce::String& paramId = args.arguments[0].toString();
            const double value = args.arguments[1];
           
            DBG("Setting " << paramId << " to " << value);
//            if (auto* parameter = self->processor.getValueTreeState().getParameter(paramId))
//                parameter->setValueNotifyingHost(value);

            return juce::var::undefined();
        },
        (void *) this
    );
    
    
    // Globals in the js env
    auto* ctx = appRoot.engine.getDuktapeContext();
    duk_push_global_object(ctx);
    duk_push_string(ctx, JucePlugin_VersionString);
    duk_put_prop_string(ctx, -2, "__VERSION__");



    setResizable(true, true);
    setResizeLimits(667, 375, 1334, 750);
    getConstrainer()->setFixedAspectRatio(1334.0 / 750.0);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (667, 375);
}

BlueprintPluginAudioProcessorEditor::~BlueprintPluginAudioProcessorEditor()
{
}

//==============================================================================
void BlueprintPluginAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), Justification::centred, 1);
}

void BlueprintPluginAudioProcessorEditor::resized()
{
    appRoot.setBounds(getLocalBounds());
}
