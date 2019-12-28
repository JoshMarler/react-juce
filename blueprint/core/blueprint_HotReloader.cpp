/*
  ==============================================================================

    blueprint_HotRealoader.cpp
    Created: 11 Dec 2019 2:20:29pm

  ==============================================================================
*/

namespace blueprint
{

//==============================================================================
HotReloader::HotReloader(std::unique_ptr<blueprint::ReactApplicationRoot> &appRoot, const juce::File &bundle, juce::Component &parentJuceComp) : root((std::unique_ptr<blueprint::ReactApplicationRoot> &)std::move(appRoot)), bundleFile(bundle), parentComp(parentJuceComp)
{

    // Sanity check
    jassert(bundle.existsAsFile());
    bundleFile = bundle;
    lastModifiedTime = bundleFile.getLastModificationTime();

    assignNewAppRoot(bundle.loadFileAsString());
    startTimer(50);
}

HotReloader::~HotReloader()
{
    stopTimer();
}

void HotReloader::timerCallback()
{
    auto lmt = bundleFile.getLastModificationTime();

    if (lmt > lastModifiedTime)
    {
        // Sanity check... again
        jassert(bundleFile.existsAsFile());

        // Remove and delete the current appRoot
        root.reset();

        // Then we assign a new one
        assignNewAppRoot(bundleFile.loadFileAsString());

        if (root)
        {
            parentComp.addAndMakeVisible(root.get());
            root->setBounds(parentComp.getLocalBounds());
        }

        lastModifiedTime = lmt;
    }
}

void HotReloader::assignNewAppRoot(const juce::String &code)
{

    root = std::make_unique<blueprint::ReactApplicationRoot>();
    root->engine.onUncaughtError = [this](const juce::String &msg, const juce::String &trace) {
        root.reset();
        DBG(msg);
        DBG(trace);
    };

    // Sanity check
    root->evaluate(code);
}
} // namespace blueprint
