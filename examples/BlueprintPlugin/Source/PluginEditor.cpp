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
    appRoot.runScript(sourceDir.getChildFile("ui/build/js/main.js"));

    setResizable(true, true);
    setResizeLimits(600, 450, 1200, 900);
    getConstrainer()->setFixedAspectRatio(600.0 / 450.0);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 450);
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
