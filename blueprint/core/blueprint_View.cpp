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

    juce::Identifier View::getRefId()
    {
        return _refId;
    }

    void View::setProperty (const juce::Identifier& name, const juce::var& value)
    {
        props.set(name, value);

        if (name == juce::Identifier("interceptClickEvents"))
        {
            int flag = value;

            switch (flag) {
                case 0:
                    setInterceptsMouseClicks(false, false);
                    break;
                case 2:
                    setInterceptsMouseClicks(true, false);
                    break;
                case 3:
                    setInterceptsMouseClicks(false, true);
                    break;
                case 1:
                default:
                    setInterceptsMouseClicks(true, true);
                    break;
            }
        }

        if (name == juce::Identifier("opacity"))
            setAlpha((double) value);
        if (name == juce::Identifier("refId"))
            _refId = juce::Identifier(value.toString());
    }

    void View::addChild (View* childView, int index)
    {
        // Add the child view to our component heirarchy.
        addAndMakeVisible(childView, index);
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
        if (props.contains("border-path"))
        {
            juce::Path p = juce::Drawable::parseSVGPath(props["border-path"].toString());

            if (props.contains("border-color"))
            {
                juce::Colour c = juce::Colour::fromString(props["border-color"].toString());
                float borderWidth = props.getWithDefault("border-width", 1.0);

                g.setColour(c);
                g.strokePath(p, juce::PathStrokeType(borderWidth));
            }

            g.reduceClipRegion(p);
        }
        else if (props.contains("border-color") && props.contains("border-width"))
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

    void View::mouseDoubleClick (const juce::MouseEvent& e)
    {
        if (ReactApplicationRoot* root = findParentComponentOfClass<ReactApplicationRoot>())
            root->dispatchViewEvent(getViewId(), "MouseDoubleClick", e.x, e.y);
    }
}
