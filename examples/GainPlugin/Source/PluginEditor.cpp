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

    // Next we just add our appRoot and kick off the app bundle.
    addAndMakeVisible(appRoot);
    appRoot.evalScript(bundle.loadFileAsString());

    // And of course set our editor size before we're done.
    setSize (400, 300);
}

GainPluginAudioProcessorEditor::~GainPluginAudioProcessorEditor()
{
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
