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

    appRoot.setRootComponent(this);
    appRoot.runScript(sourceDir.getChildFile("ui/build/static/js/main.js"));

    testView = std::make_unique<blueprint::View>();
    testView2 = std::make_unique<blueprint::View>();
    testView->setProperty("flex", 1.0);
    testView->setProperty("debug", 1.0);
    testView->setProperty("background-color", "ff272777");
    testView2->setProperty("max-height", 200.0);
    testView2->setProperty("flex", 1.0);
    testView2->setProperty("background-color", "ff772727");
    addAndMakeVisible(testView.get());
    testView->appendChild(testView2.get());

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
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
    testView->performLayout(getLocalBounds().toFloat());
}
