/*
  ==============================================================================

    View.cpp
    Created: 26 Nov 2018 3:38:37am

  ==============================================================================
*/

#include "View.h"
#include "Utils.h"

#include <climits>


namespace reactjuce
{

    namespace detail
    {
        static juce::var getMouseEventRelatedTarget(const juce::MouseEvent& e, const View& view)
        {
            juce::Component *topParent              = view.getTopLevelComponent();
            const juce::MouseEvent topRelativeEvent = e.getEventRelativeTo(topParent);

            juce::Component *componentUnderMouse = topParent->getComponentAt(topRelativeEvent.x, topRelativeEvent.y);

            if (auto v = dynamic_cast<View*>(componentUnderMouse))
                return v->getViewId();

            // return null relatedTarget if event occurred from a non-View component.
            return {};
        }

        /** A little helper for DynamicObject construction. */
        static juce::var makeViewEventObject (const juce::NamedValueSet& props, const View& view)
        {
            auto* o = new juce::DynamicObject();

            for (auto& pair : props)
                o->setProperty(pair.name, pair.value);

            o->setProperty("target", view.getViewId());

            //TODO: Add timeStamp.

            return juce::var(o);
        }

        /** Another little helper for DynamicObject construction. */
        static juce::var makeViewEventObject (const juce::MouseEvent& me, const View &view)
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
        static juce::var makeViewEventObject (const juce::KeyPress& ke, const View &view)
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

        if (name == interceptClickEventsProp)
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

        if (name == onKeyPressProp)
            setWantsKeyboardFocus(true);

        if (name == opacityProp)
            setAlpha(static_cast<float> (value));

        if (name == refIdProp)
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
        if (props.contains(transformMatrixProp))
        {
            const juce::var& matrix = props[transformMatrixProp];
            if(matrix.isArray() && matrix.getArray()->size() >= 16) {
              const juce::Array<juce::var> &m = *matrix.getArray();

              auto cxRelParent = cachedFloatBounds.getX() + cachedFloatBounds.getWidth() * 0.5f;
              auto cyRelParent = cachedFloatBounds.getY() + cachedFloatBounds.getHeight() * 0.5f;

              const auto translateToOrigin = juce::AffineTransform::translation(cxRelParent * -1.0f, cyRelParent * -1.0f);
              // set 2d homogeneous matrix using 3d homogeneous matrix
              const auto transform = juce::AffineTransform(
                m[0], m[1], m[3],
                m[4], m[5], m[7]
              );
              const auto translateFromOrigin = juce::AffineTransform::translation(cxRelParent, cyRelParent);

              setTransform(translateToOrigin.followedBy(transform).followedBy(translateFromOrigin));
            }
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
        if (props.contains(borderPathProp))
        {
            juce::Path p = juce::Drawable::parseSVGPath(props[borderPathProp].toString());

            if (props.contains(borderColorProp))
            {
                juce::StringRef colorString = props[borderColorProp].toString();
                juce::Colour c = juce::Colour::fromString(colorString);
                float borderWidth = props.getWithDefault(borderWidthProp, 1.0);

                g.setColour(c);
                g.strokePath(p, juce::PathStrokeType(borderWidth));
            }

            g.reduceClipRegion(p);
        }
        else if (props.contains(borderColorProp) && props.contains(borderWidthProp))
        {
            juce::Path border;
            juce::StringRef colorString = props[borderColorProp].toString();
            auto c = juce::Colour::fromString(colorString);
            float borderWidth = props[borderWidthProp];

            // Note this little bounds trick. When a Path is stroked, the line width extends
            // outwards in both directions from the coordinate line. If the coordinate
            // line is the exact bounding box then the component clipping makes the corners
            // appear to have different radii on the interior and exterior of the box.
            auto borderBounds = getLocalBounds().toFloat().reduced(borderWidth * 0.5f);
            auto width  = borderBounds.getWidth();
            auto height = borderBounds.getHeight();
            auto minLength = juce::jmin(width, height);
            float borderRadius = getResolvedLengthProperty(borderRadiusProp.toString(), minLength);

            border.addRoundedRectangle(borderBounds, borderRadius);
            g.setColour(c);
            g.strokePath(border, juce::PathStrokeType(borderWidth));
            g.reduceClipRegion(border);
        }

        if (props.contains(backgroundColorProp))
        {
            auto colorProp = props[backgroundColorProp];
            auto colorVariant = detail::makeColorVariant(colorProp, getLocalBounds());
            if(const auto color (std::get_if<juce::Colour>(&colorVariant)); color)
            {
                if (!color->isTransparent())
                    g.fillAll(*color);
            }
            else if(const auto gradient (std::get_if<juce::ColourGradient>(&colorVariant)); gradient)
            {
                g.setGradientFill(*gradient);
                g.fillAll();
            }
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

    void View::mouseEnter (const juce::MouseEvent& e)
    {
        dispatchViewEvent("onMouseEnter", detail::makeViewEventObject(e, *this));
    }

    void View::mouseExit (const juce::MouseEvent& e)
    {
        dispatchViewEvent("onMouseLeave", detail::makeViewEventObject(e, *this));
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

    void View::exportMethod(const juce::String &method, juce::var::NativeFunction fn)
    {
        JUCE_ASSERT_MESSAGE_THREAD
        nativeMethods[method] = std::move(fn);
    }

    juce::var View::invokeMethod(const juce::String &method, const juce::var::NativeFunctionArgs &args)
    {
        JUCE_ASSERT_MESSAGE_THREAD
        auto it = nativeMethods.find(method);

        if (it != nativeMethods.end())
            return it->second(args);

        throw std::logic_error("Caller attempted to invoke a non-existent View method");
    }
}
