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

    void View::addBorderSubpath (juce::Path& p)
    {
        // Note this little bounds trick. When a Path is stroked, the line width extends
        // outwards in both directions from the coordinate line. If the coordinate
        // line is the exact bounding box then the component clipping makes the corners
        // appear to have different radii on the interior and exterior of the box.
        float borderWidth = props.getWithDefault("border-width", 1.0f);
        auto borderBounds = getLocalBounds().toFloat().reduced(borderWidth * 0.5f);

        const float x = borderBounds.getX();
        const float y = borderBounds.getY();
        const float width = borderBounds.getWidth();
        const float height = borderBounds.getHeight();
        const float minLength = std::min(width, height);
        const float halfMinLength = minLength * 0.5f;
        const float x2 = x + width;
        const float y2 = y + height;
        const float pi = juce::MathConstants<float>::pi;

        float rtl = getResolvedLengthProperty("border-top-left-radius", minLength);
        float rtr = getResolvedLengthProperty("border-top-right-radius", minLength);
        float rbr = getResolvedLengthProperty("border-bottom-right-radius", minLength);
        float rbl = getResolvedLengthProperty("border-bottom-left-radius", minLength);

        if (rtl >= 0.0f)
        {
            rtl = std::min(halfMinLength, rtl);
            p.startNewSubPath(x, y + rtl);
            p.addArc(x, y, 2.0 * rtl, 2.0 * rtl, 1.5 * pi, 2.0 * pi);
        }
        else
        {
            rtl = std::min(halfMinLength, std::abs(rtl));
            p.startNewSubPath(x, y + rtl);
            p.addArc(x - rtl, y - rtl, 2.0 * rtl, 2.0 * rtl, 1.0 * pi, 0.5 * pi);
        }

        if (rtr >= 0.0f)
        {
            rtr = std::min(halfMinLength, rtr);
            p.lineTo(x2 - rtr, y);
            p.addArc(x2 - 2.0 * rtr, y, 2.0 * rtr, 2.0 * rtr, 2.0 * pi, 2.5 * pi);
        }
        else
        {
            rtr = std::min(halfMinLength, std::abs(rtr));
            p.lineTo(x2 - rtr, y);
            p.addArc(x2 - rtr, y - rtr, 2.0 * rtr, 2.0 * rtr, 1.5 * pi, 1.0 * pi);
        }

        if (rbr >= 0.0f)
        {
            rbr = std::min(halfMinLength, rbr);
            p.lineTo(x2, y2 - rbr);
            p.addArc(x2 - 2.0 * rbr, y2 - 2.0 * rbr, 2.0 * rbr, 2.0 * rbr, 2.5 * pi, 3.0 * pi);
        }
        else
        {
            rbr = std::min(halfMinLength, std::abs(rbr));
            p.lineTo(x2, y2 - rbr);
            p.addArc(x2 - rbr, y2 - rbr, 2.0 * rbr, 2.0 * rbr, 2.0 * pi, 1.5 * pi);
        }

        if (rbl >= 0.0f)
        {
            rbl = std::min(halfMinLength, rbl);
            p.lineTo(x + rbl, y2);
            p.addArc(x, y2 - 2.0 * rbl, 2.0 * rbl, 2.0 * rbl, 1.0 * pi, 1.5 * pi);
        }
        else
        {
            rbl = std::min(halfMinLength, std::abs(rbl));
            p.lineTo(x + rbl, y2);
            p.addArc(x - rbl, y2 - rbl, 2.0 * rbl, 2.0 * rbl, 0.5 * pi, 0);
        }

        p.closeSubPath();
    }

    void View::paint (juce::Graphics& g)
    {
        if (props.contains("border-color"))
        {
            juce::Path border;
            juce::Colour c = juce::Colour::fromString(props["border-color"].toString());
            float borderWidth = props.getWithDefault("border-width", 1.0f);

            g.setColour(c);
            addBorderSubpath(border);
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

    void View::resized()
    {
        jassert (ReactApplicationRoot::singletonInstance != nullptr);

        ReactApplicationRoot* root = ReactApplicationRoot::singletonInstance;
        root->dispatchViewEvent(getViewId(), "Measure", getWidth(), getHeight());
    }

}
