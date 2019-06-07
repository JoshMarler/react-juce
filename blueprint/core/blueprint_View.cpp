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

        if (name == juce::Identifier("interceptClickEvents"))
            setInterceptsMouseClicks((bool) value, (bool) value);

        if (name == juce::Identifier("opacity"))
            setAlpha((double) value);
    }

    void View::appendChild (View* childView)
    {
        // Add the child view to our component heirarchy.
        addAndMakeVisible(childView);
    }

    void View::setFloatBounds(juce::Rectangle<float> bounds)
    {
        cachedFloatBounds = bounds;

        // Update transforms
        if (props.contains("transform-rotate"))
        {
            float cxRelParent = cachedFloatBounds.getX() + cachedFloatBounds.getWidth() * 0.5f;
            float cyRelParent = cachedFloatBounds.getY() + cachedFloatBounds.getHeight() * 0.5f;
            double angle = props["transform-rotate"];

            setTransform(juce::AffineTransform::rotation(angle, cxRelParent, cyRelParent));
        }
    }

    //==============================================================================
    float View::getResolvedLengthProperty (const juce::String& name, float axisLength)
    {
        float ret = 0.0;

        if (props.contains(name))
        {
            const auto& v = props[name];

            if (v.isString() && v.toString().trim().endsWithChar('%'))
            {
                float pctVal = v.toString().retainCharacters("-1234567890.").getFloatValue();
                ret = axisLength * (pctVal / 100.0f);
            }
            else
            {
                ret = (float) v;
            }
        }

        return ret;
    }

    void View::paint (juce::Graphics& g)
    {
        if (props.contains("border-color") && props.contains("border-width"))
        {
            juce::Path border;
            juce::Colour c = juce::Colour::fromString(props["border-color"].toString());
            float borderWidth = props["border-width"];

            // Note this little bounds trick. When a Path is stroked, the line width extends
            // outwards in both directions from the coordinate line. If the coordinate
            // line is the exact bounding box then the component clipping makes the corners
            // appear to have different radii on the interior and exterior of the box.
            auto borderBounds = getLocalBounds().toFloat().reduced(borderWidth * 0.5f);
            const float width = borderBounds.getWidth();
            const float height = borderBounds.getHeight();
            const float minLength = std::min(width, height);
            float borderRadius = getResolvedLengthProperty("border-radius", minLength);

            // TODO: Here we have a limited mechanism for drawing a rectangular border with
            // or without rounded corners, but there's no reason we couldn't support arbitrary
            // border shapes with something like a "border-path" property, where the property
            // value is a Path string (M 0 50 L 50 50...). Then we could support arbitrary border
            // shapes while still hitting the `reduceClipRegion` for the background color fill.
            // Could also use that Path shape for overriding the hitTest implementation so that
            // only hits within the border path shape satisfy a "hit."
            border.addRoundedRectangle(borderBounds, borderRadius);
            g.setColour(c);
            g.strokePath(border, juce::PathStrokeType(borderWidth));
            g.reduceClipRegion(border);
        }

        if (props.contains("background-color"))
        {
            juce::Colour c = juce::Colour::fromString(props["background-color"].toString());

            if (!c.isTransparent())
                g.fillAll(c);
        }

    }

    //==============================================================================
    void View::resized()
    {
        auto w = cachedFloatBounds.getWidth();
        auto h = cachedFloatBounds.getHeight();

        if (ReactApplicationRoot* root = findParentComponentOfClass<ReactApplicationRoot>())
            root->dispatchViewEvent(getViewId(), "Measure", w, h);
    }

    void View::mouseDown (const juce::MouseEvent& e)
    {
        if (ReactApplicationRoot* root = findParentComponentOfClass<ReactApplicationRoot>())
            root->dispatchViewEvent(getViewId(), "MouseDown", e.x, e.y);
    }

    void View::mouseUp (const juce::MouseEvent& e)
    {
        if (ReactApplicationRoot* root = findParentComponentOfClass<ReactApplicationRoot>())
            root->dispatchViewEvent(getViewId(), "MouseUp", e.x, e.y);
    }

    void View::mouseDrag (const juce::MouseEvent& e)
    {
        float mouseDownX = e.mouseDownPosition.getX();
        float mouseDownY = e.mouseDownPosition.getY();

        if (ReactApplicationRoot* root = findParentComponentOfClass<ReactApplicationRoot>())
            root->dispatchViewEvent(getViewId(), "MouseDrag", e.x, e.y, mouseDownX, mouseDownY);
    }
}
