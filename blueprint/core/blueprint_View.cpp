namespace blueprint
{
    namespace detail
    {
        juce::var getMouseEventRelatedTarget(const juce::MouseEvent& e, const blueprint::View& view)
        {
            auto* topParent = view.getTopLevelComponent();
            jassert (topParent != nullptr);

            const auto topRelativeEvent = e.getEventRelativeTo(topParent);

            auto* componentUnderMouse = topParent->getComponentAt (topRelativeEvent.x, topRelativeEvent.y);
            if (auto* v = dynamic_cast<blueprint::View*>(componentUnderMouse))
                return v->getViewId();

            // return null relatedTarget if event occurred from a non-View component.
            return {};
        }

        /** A little helper for DynamicObject construction. */
        juce::var makeViewEventObject (const juce::NamedValueSet& props, const blueprint::View& view)
        {
            auto* o = new juce::DynamicObject();

            for (auto& pair : props)
                o->setProperty(pair.name, pair.value);

            o->setProperty("target", view.getViewId());

            //TODO: Add timeStamp.

            return juce::var(o);
        }

        /** Another little helper for DynamicObject construction. */
        juce::var makeViewEventObject (const juce::MouseEvent& me, const blueprint::View &view)
        {
            // TODO: Get all of it!
            return makeViewEventObject({
                {"x", me.x},
                {"y", me.y},
                {"screenX", me.getScreenX()},
                {"screenY", me.getScreenY()},
                {"relatedTarget", getMouseEventRelatedTarget(me, view)}
            }, view);
        }

        /** And another little helper for DynamicObject construction. */
        juce::var makeViewEventObject (const juce::KeyPress& ke, const blueprint::View &view)
        {
            // TODO: Get all of it!
            return makeViewEventObject({
                {"key", juce::String(ke.getTextCharacter())},
                {"keyCode", ke.getKeyCode()},
            }, view);
        }
    }

    //==============================================================================
    ViewId View::getViewId() const
    {
        return juce::DefaultHashFunctions::generateHash(_viewId, INT_MAX);
    }

    juce::Identifier View::getRefId() const
    {
        return _refId;
    }

    void View::setProperty (const juce::Identifier& name, const juce::var& value)
    {
        props.set(name, value);

        if (name == juce::StringRef ("interceptClickEvents"))
        {
            switch (static_cast<int> (value))
            {
                case 0:     setInterceptsMouseClicks (false, false);  break;
                case 1:     setInterceptsMouseClicks (true,  true);   break;
                case 2:     setInterceptsMouseClicks (true,  false);  break;
                case 3:     setInterceptsMouseClicks (false, true);   break;

                default:    setInterceptsMouseClicks (true,  true);   break;
            }
        }

        if (name == juce::StringRef("onKeyPress"))
            setWantsKeyboardFocus(true);

        if (name == juce::StringRef("opacity"))
            setAlpha(static_cast<float> (value));

        if (name == juce::StringRef("refId"))
            _refId = juce::Identifier(value.toString());
    }

    void View::addChild (View* childView, int index)
    {
        addAndMakeVisible(childView, index);
    }

    void View::setFloatBounds(juce::Rectangle<float> bounds)
    {
        static const juce::Identifier transformMatrix("transform-matrix");

        cachedFloatBounds = bounds;

        // Update transforms
        if (props.contains(transformMatrix))
        {
            const auto& matrix = props[transformMatrix];

            if(matrix.isArray() && matrix.getArray()->size() >= 16)
            {
                const auto &m = *matrix.getArray();

                auto cxRelParent = cachedFloatBounds.getX() + cachedFloatBounds.getWidth() * 0.5f;
                auto cyRelParent = cachedFloatBounds.getY() + cachedFloatBounds.getHeight() * 0.5f;

                const auto translateToOrigin = juce::AffineTransform::translation(cxRelParent * -1.0f, cyRelParent * -1.0f);

                // set 2d homogeneous matrix using 3d homogeneous matrix
                const auto transform = juce::AffineTransform(m[0], m[1], m[3], m[4], m[5], m[7]);
                const auto translateFromOrigin = juce::AffineTransform::translation(cxRelParent, cyRelParent);

                setTransform(translateToOrigin.followedBy(transform).followedBy(translateFromOrigin));
            }
        }
    }

    //==============================================================================
    float View::getResolvedLengthProperty (const juce::String& name, float axisLength)
    {
        if (props.contains(name))
        {
            const auto& v = props[name];

            if (v.isString() && v.toString().trim().endsWithChar('%'))
            {
                const auto pctVal = v.toString().retainCharacters("-1234567890.").getFloatValue();
                return axisLength * (pctVal / 100.0f);
            }

            return (float) v;
        }

        return 0.0f;
    }

    void View::paint (juce::Graphics& g)
    {
        if (props.contains("border-path"))
        {
            auto p = juce::Drawable::parseSVGPath(props["border-path"].toString());

            if (props.contains("border-color"))
            {
                auto c = juce::Colour::fromString(props["border-color"].toString());
                auto borderWidth = (float) props.getWithDefault("border-width", 1.0f);

                g.setColour(c);
                g.strokePath(p, juce::PathStrokeType(borderWidth));
            }

            g.reduceClipRegion(p);
        }
        else if (props.contains("border-color") && props.contains("border-width"))
        {
            const auto c = juce::Colour::fromString(props["border-color"].toString());
            const auto borderWidth = (float) props["border-width"];

            // Note this little bounds trick. When a Path is stroked, the line width extends
            // outwards in both directions from the coordinate line. If the coordinate
            // line is the exact bounding box then the component clipping makes the corners
            // appear to have different radii on the interior and exterior of the box.
            const auto borderBounds = getLocalBounds().toFloat().reduced(borderWidth * 0.5f);
            const auto width  = borderBounds.getWidth();
            const auto height = borderBounds.getHeight();
            const auto minLength = std::min(width, height);
            const auto borderRadius = getResolvedLengthProperty("border-radius", minLength);

            juce::Path border;
            border.addRoundedRectangle(borderBounds, borderRadius);

            g.setColour(c);
            g.strokePath (border, juce::PathStrokeType { borderWidth });
            g.reduceClipRegion(border);
        }

        if (props.contains("background-color"))
        {
            const auto c = juce::Colour::fromString(props["background-color"].toString());
            if (! c.isTransparent())
                g.fillAll(c);
        }
    }

    //==============================================================================
    void View::resized()
    {
        dispatchViewEvent("onMeasure", detail::makeViewEventObject({
            {"width", cachedFloatBounds.getWidth() },
            {"height", cachedFloatBounds.getHeight() }
        }, *this));
    }

    void View::mouseDown (const juce::MouseEvent& e)
    {
        dispatchViewEvent("onMouseDown", detail::makeViewEventObject(e, *this));
    }

    void View::mouseUp (const juce::MouseEvent& e)
    {
        dispatchViewEvent("onMouseUp", detail::makeViewEventObject(e, *this));
    }

    void View::mouseDrag (const juce::MouseEvent& e)
    {
        // TODO: mouseDrag isn't a dom event... is it?
        dispatchViewEvent("onMouseDrag", detail::makeViewEventObject(e, *this));
    }

    void View::mouseDoubleClick (const juce::MouseEvent& e)
    {
        dispatchViewEvent("onMouseDoubleClick", detail::makeViewEventObject(e, *this));
    }

    bool View::keyPressed (const juce::KeyPress& key)
    {
        dispatchViewEvent("onKeyPress", detail::makeViewEventObject(key, *this));

        // We always inform the underlying juce::Component that we've consumed the event,
        // because we manually bubble a SyntheticEvent wrapper up the javascript view tree.
        // However, because the React app may be only a subtree of the overall app architecture,
        // we skip up to the ReactApplicationRoot parent and restart the keypress event propagation
        // up there.
        if (auto* parent = findParentComponentOfClass<ReactApplicationRoot>())
            parent->keyPressed(key);

        return true;
    }

    void View::dispatchViewEvent (const juce::String& eventType, const juce::var& e)
    {
        JUCE_ASSERT_MESSAGE_THREAD

        if (auto *parent = findParentComponentOfClass<ReactApplicationRoot>())
            parent->dispatchViewEvent(getViewId(), eventType, e);
    }
}
