/*
  ==============================================================================

    blueprint_View.cpp
    Created: 26 Nov 2018 3:38:37am

  ==============================================================================
*/


namespace blueprint
{

    //==============================================================================
    ViewId View::getViewId()
    {
        return juce::DefaultHashFunctions::generateHash(_viewId, INT_MAX);
    }

    void View::setProperty (const juce::Identifier& name, const juce::var& value)
    {
        props.set(name, value);
    }

    void View::appendChild (View* childView)
    {
        // Add the child view to our component heirarchy.
        addAndMakeVisible(childView);
    }

    //==============================================================================
    void View::paint (juce::Graphics& g)
    {
        if (props.contains("background-color"))
        {
                juce::Colour bgColour = juce::Colour::fromString(props["background-color"].toString());

            if (!bgColour.isTransparent())
                g.fillAll(bgColour);
        }
    }

}
