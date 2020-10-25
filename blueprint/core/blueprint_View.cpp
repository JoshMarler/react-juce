/*
  ==============================================================================

    blueprint_View.cpp
    Created: 26 Nov 2018 3:38:37am

  ==============================================================================
*/


namespace blueprint
{

    namespace detail
    {

        /** A little helper for DynamicObject construction. */
        juce::var makeViewEventObject (const juce::NamedValueSet& props)
        {
            auto* o = new juce::DynamicObject();

            for (auto& pair : props)
                o->setProperty(pair.name, pair.value);

            return juce::var(o);
        }

        /** Another little helper for DynamicObject construction. */
        juce::var makeViewEventObject (const juce::MouseEvent& me)
        {
            // TODO: Get all of it!
            return makeViewEventObject({
                {"x", me.x},
                {"y", me.y},
                {"screenX", me.getScreenX()},
                {"screenY", me.getScreenY()},
            });
        }

        /** And another little helper for DynamicObject construction. */
        juce::var makeViewEventObject (const juce::KeyPress& ke)
        {
            // TODO: Get all of it!
            return makeViewEventObject({
                {"key", juce::String(ke.getTextCharacter())},
                {"keyCode", ke.getKeyCode()},
            });
        }

    }

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

        if (name == juce::StringRef ("interceptClickEvents"))
        {
            switch (static_cast<int> (value))
            {
                case 0:      setInterceptsMouseClicks (false, false);  break;
                case 1:      setInterceptsMouseClicks (true,  true);   break;
                case 2:      setInterceptsMouseClicks (true,  false);  break;
                case 3:      setInterceptsMouseClicks (false, true);   break;

                default:     setInterceptsMouseClicks (true,  true);   break;
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
        // Add the child view to our component heirarchy.
        addAndMakeVisible(childView, index);
    }

    void View::setFloatBounds(juce::Rectangle<float> bounds)
    {
        cachedFloatBounds = bounds;

        // Update transforms
        if (props.contains("transform-rotate"))
        {
            auto cxRelParent = cachedFloatBounds.getX() + cachedFloatBounds.getWidth() * 0.5f;
            auto cyRelParent = cachedFloatBounds.getY() + cachedFloatBounds.getHeight() * 0.5f;
            auto angle = static_cast<float> (props["transform-rotate"]);

            setTransform(juce::AffineTransform::rotation(angle, cxRelParent, cyRelParent));
        }
        if (props.contains("transform-scaleY"))
        {
            auto scaleY = static_cast<float> (props["transform-scaleY"]);

            setTransform(juce::AffineTransform::scale(1.0f, scaleY));
        }
        if (props.contains("transform-scaleX"))
        {
            auto scaleX = static_cast<float> (props["transform-scaleX"]);
            setTransform(juce::AffineTransform::scale(scaleX, 1.0f));
        }
        if (props.contains("transform-scale"))
        {
            float scaleX = props["transform-scale"].getArray()->getReference(0);
            float scaleY = props["transform-scale"].getArray()->getReference(1);
            setTransform(juce::AffineTransform::scale(scaleX, scaleY));
        }
        if (props.contains("transform-translate"))
        {
            float deltaX = props["transform-translate"].getArray()->getReference(0);
            float deltaY = props["transform-translate"].getArray()->getReference(1);
            setTransform(juce::AffineTransform::translation(deltaX, deltaY));
        }
        
          if (props.contains("transform-scale-translate"))
        {
            float scaleX = props["transform-scale-translate"].getArray()->getReference(0);
            float scaleY = props["transform-scale-translate"].getArray()->getReference(1);
            float deltaX = props["transform-scale-translate"].getArray()->getReference(2);
            float deltaY = props["transform-scale-translate"].getArray()->getReference(3);
            setTransform(juce::AffineTransform::scale(scaleX, scaleY).translated(deltaX, deltaY));
        }

         if (props.contains("transform-scale-pivot"))
        {
            float scaleX = props["transform-scale-pivot"].getArray()->getReference(0);
            float scaleY = props["transform-scale-pivot"].getArray()->getReference(1);
            float pivotX = props["transform-scale-pivot"].getArray()->getReference(2);
            float pivotY = props["transform-scale-pivot"].getArray()->getReference(3);
            setTransform(juce::AffineTransform::scale(scaleX, scaleY,pivotX,pivotY));
        }


        
    }

    //==============================================================================
    float View::getResolvedLengthProperty (const juce::String& name, float axisLength)
    {
        float ret = 0;

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
            auto c = juce::Colour::fromString(props["border-color"].toString());
            float borderWidth = props["border-width"];

            // Note this little bounds trick. When a Path is stroked, the line width extends
            // outwards in both directions from the coordinate line. If the coordinate
            // line is the exact bounding box then the component clipping makes the corners
            // appear to have different radii on the interior and exterior of the box.
            auto borderBounds = getLocalBounds().toFloat().reduced(borderWidth * 0.5f);
            auto width  = borderBounds.getWidth();
            auto height = borderBounds.getHeight();
            auto minLength = std::min(width, height);
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

        dispatchViewEvent("onMeasure", detail::makeViewEventObject({
            {"width", w},
            {"height", h}
        }));
    }

    void View::mouseMove (const juce::MouseEvent& e)
    {
        dispatchViewEvent("onMouseMove", detail::makeViewEventObject(e));
    }

    void View::mouseEnter (const juce::MouseEvent& e)
    {
        dispatchViewEvent("onMouseEnter", detail::makeViewEventObject(e));
    }

    void View::mouseExit (const juce::MouseEvent& e)
    {
        dispatchViewEvent("onMouseExit", detail::makeViewEventObject(e));
    }

    void View::mouseDown (const juce::MouseEvent& e)
    {
        dispatchViewEvent("onMouseDown", detail::makeViewEventObject(e));
    }

    void View::mouseUp (const juce::MouseEvent& e)
    {
        dispatchViewEvent("onMouseUp", detail::makeViewEventObject(e));
    }

    void View::mouseDrag (const juce::MouseEvent& e)
    {
        // TODO: mouseDrag isn't a dom event... is it?
        dispatchViewEvent("onMouseDrag", detail::makeViewEventObject(e));
    }


    void View::mouseDoubleClick (const juce::MouseEvent& e)
    {
        dispatchViewEvent("onMouseDoubleClick", detail::makeViewEventObject(e));
    }

    bool View::keyPressed (const juce::KeyPress& key)
    {
        dispatchViewEvent("onKeyPress", detail::makeViewEventObject(key));

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

        if (props.contains(eventType) && props[eventType].isMethod())
        {
            std::array<juce::var, 1> vargs { e };
            juce::var::NativeFunctionArgs nfArgs (juce::var(), vargs.data(), static_cast<int>(vargs.size()));

            try
            {
                std::invoke(props[eventType].getNativeFunction(), nfArgs);
            }
            catch (const EcmascriptEngine::Error& err)
            {
                if (auto* parent = findParentComponentOfClass<ReactApplicationRoot>())
                    return parent->handleRuntimeError(err);

                // If we coudln't find a parent that can handle it, rethrow
                throw err;
            }
        }
    }

}
