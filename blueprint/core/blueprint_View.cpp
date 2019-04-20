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
            juce::Colour c = juce::Colour::fromString(props["background-color"].toString());

            if (!c.isTransparent())
                g.fillAll(c);
        }

        if (props.contains("border-color"))
        {
            juce::Colour c = juce::Colour::fromString(props["border-color"].toString());

            float width = props.getWithDefault("border-width", 1.f);
            float radius = props.getWithDefault("border-radius", 0.f);

            juce::Path border;

            // Note this little bounds trick. When a Path is stroked, the line width extends
            // outwards in both directions from the coordinate line. If the coordinate
            // line is the exact bounding box then the component clipping makes the corners
            // appear to have different radii on the interior and exterior of the box.
            auto borderBounds = getLocalBounds().toFloat().reduced(width * 0.5);

            border.addRoundedRectangle(borderBounds, radius);
            g.setColour(c);
            g.strokePath(border, juce::PathStrokeType(width));
        }
    }

}
