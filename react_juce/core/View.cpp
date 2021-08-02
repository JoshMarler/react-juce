/*
  ==============================================================================

    View.cpp
    Created: 26 Nov 2018 3:38:37am

  ==============================================================================
*/

#include "View.h"
#include "Utils.h"

#include <climits>

namespace {
    enum BorderEdge
    {
        TOP = 0,
        RIGHT,
        BOTTOM,
        LEFT
    };

    float getResolvedFloatProperty (const juce::var& val, float axisLength)
    {
        float ret;

        if (val.isString() && val.toString().trim().endsWithChar('%'))
        {
            float pctVal = val.toString().retainCharacters("-1234567890.").getFloatValue();
            ret = axisLength * (pctVal / 100.0f);
        }
        else
            ret = static_cast<float>(val);

        return ret;
    }

    // Generate a path for a single border edge. With a path offset of
    // 0, the path runs along the inner edge of the lines we draw, so it
    // can be used to build a clip region for the content inside the border.
    // The path offset pulls it out; we set that to half the line width to
    // get a path in the middle of the line suitable to stroke to draw the border.
    juce::Path makeEdgePath(BorderEdge edge, float width, float pathOffset, float prevWidth, float nextWidth, float startRadius, float endRadius, float borderWidth, float borderHeight)
    {
        juce::Path res;
        const auto Pi = juce::MathConstants<float>::pi;

        if (startRadius > 0)
        {
            switch(edge)
            {
            case BorderEdge::TOP:
                res.addCentredArc(prevWidth + startRadius, width + startRadius,
                                  startRadius + pathOffset, startRadius + pathOffset,
                                  0,
                                  1.75 * Pi, 2.0 * Pi,
                                  true);
                break;
            case BorderEdge::RIGHT:
                res.addCentredArc(borderWidth - width - startRadius, prevWidth + startRadius,
                                  startRadius + pathOffset, startRadius + pathOffset,
                                  0,
                                  0.25 * Pi, 0.5 * Pi,
                                  true);
                break;
            case BorderEdge::BOTTOM:
                res.addCentredArc(borderWidth - prevWidth - startRadius, borderHeight - width  - startRadius,
                                  startRadius + pathOffset, startRadius + pathOffset,
                                  0,
                                  0.75 * Pi, Pi,
                                  true);
                break;
            case BorderEdge::LEFT:
                res.addCentredArc(width + startRadius, borderHeight - prevWidth - startRadius,
                                  startRadius + pathOffset, startRadius + pathOffset,
                                  0,
                                  1.25 * Pi, 1.5 * Pi,
                                  true);
                break;
            }
        }
        else
        {
            switch(edge)
            {
            case BorderEdge::TOP:
                res.startNewSubPath(prevWidth + startRadius, width - pathOffset);
                break;
            case BorderEdge::RIGHT:
                res.startNewSubPath(borderWidth - width + pathOffset, prevWidth + startRadius);
                break;
            case BorderEdge::BOTTOM:
                res.startNewSubPath(borderWidth - prevWidth - startRadius, borderHeight - width + pathOffset);
                break;
            case BorderEdge::LEFT:
                res.startNewSubPath(width - pathOffset, borderHeight - prevWidth - startRadius);
                break;
            }
        }

        if (endRadius > 0)
        {
            switch(edge)
            {
            case BorderEdge::TOP:
                res.addCentredArc(borderWidth - nextWidth - endRadius, width + endRadius,
                                  endRadius + pathOffset, endRadius + pathOffset,
                                  0,
                                  0, 0.25 * Pi);
                break;
            case BorderEdge::RIGHT:
                res.addCentredArc(borderWidth - width - endRadius, borderHeight - nextWidth - endRadius,
                                  endRadius + pathOffset, endRadius + pathOffset,
                                  0,
                                  0.5 * Pi, 0.75 * Pi);
                break;
            case BorderEdge::BOTTOM:
                res.addCentredArc(nextWidth + endRadius, borderHeight - width - endRadius,
                                  endRadius + pathOffset, endRadius + pathOffset,
                                  0,
                                  Pi, 1.25 * Pi);
                break;
            case BorderEdge::LEFT:
                res.addCentredArc(width + endRadius, nextWidth + endRadius,
                                  endRadius + pathOffset, endRadius + pathOffset,
                                  0,
                                  1.5 * Pi, 1.75 * Pi);
                break;
            }
        }
        else
        {
            // When joining no-radius corners, a browser will mitre the corners.
            // For simplicity, we're taking an easier route - if we have a corner
            // with two non-zero width borders, a designated border always 'wins'.
            // The top border draw the top right corner, the right border the
            // bottom right corner etc.
            switch(edge)
            {
            case BorderEdge::TOP:
                res.lineTo(borderWidth, width - pathOffset);
                break;
            case BorderEdge::RIGHT:
                res.lineTo(borderWidth - width + pathOffset, borderHeight);
                break;
            case BorderEdge::BOTTOM:
                res.lineTo(0, borderHeight - width + pathOffset);
                break;
            case BorderEdge::LEFT:
                res.lineTo(width - pathOffset, 0);
                break;
            }
        }

        return res;
    }
}

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
    void View::paint (juce::Graphics& g)
    {
        if (props.contains(borderPathProp))
        {
            juce::Path p = juce::Drawable::parseSVGPath(props[borderPathProp].toString());

            if (props.contains(borderInfoProp))
            {
                juce::StringRef colorString = props[borderInfoProp]["color"][0].toString();
                juce::Colour c = juce::Colour::fromString(colorString);
                float borderWidth = props[borderInfoProp]["width"][0];
                if (borderWidth == 0)
                    borderWidth = 1.0;

                g.setColour(c);
                g.strokePath(p, juce::PathStrokeType(borderWidth));
            }

            g.reduceClipRegion(p);
        }
        else if (props.contains(borderInfoProp))
        {
            auto borderBounds = getLocalBounds().toFloat();
            auto borderWidth = borderBounds.getWidth();
            auto borderHeight = borderBounds.getHeight();
            auto minWidthHeight = juce::jmin(borderWidth, borderHeight);
            float widths[LEFT + 1];
            float radii[LEFT + 1];
            juce::StringRef colors[LEFT + 1];
            juce::StringRef styles[LEFT + 1];

            for (int edgeNo = TOP; edgeNo <= LEFT; ++edgeNo)
            {
                colors[edgeNo] = props[borderInfoProp]["color"][edgeNo].toString();
                radii[edgeNo] = getResolvedFloatProperty(props[borderInfoProp]["radius"][edgeNo], minWidthHeight);
                styles[edgeNo] = props[borderInfoProp]["style"][edgeNo].toString();
                widths[edgeNo] = props[borderInfoProp]["width"][edgeNo];
            }

            // Path and stroke the border edges.
            for (int edgeNo = TOP; edgeNo <= LEFT; ++edgeNo)
            {
                float width = widths[edgeNo];
                if (width == 0)
                    continue;

                int prevEdgeNo = edgeNo == TOP ? LEFT : edgeNo - 1;
                int nextEdgeNo = edgeNo == LEFT ? TOP : edgeNo + 1;
                float prevWidth = widths[prevEdgeNo];
                float nextWidth = widths[nextEdgeNo];
                float startRadius = radii[edgeNo];
                float endRadius = radii[nextEdgeNo];

                juce::Path p = makeEdgePath(static_cast<BorderEdge>(edgeNo),
                                            width, width / 2,
                                            prevWidth, nextWidth,
                                            startRadius, endRadius,
                                            borderWidth, borderHeight);

                auto color = juce::Colour::fromString(colors[edgeNo]);
                g.setColour(color);
                auto strokeType = juce::PathStrokeType(width);
                juce::Path strokedPath;
                if (styles[edgeNo] == juce::String("dotted"))
                {
                    float dash[] = { 1.0, 1.0 };
                    strokeType.createDashedStroke(strokedPath, p, dash, 2);
                }
                else if (styles[edgeNo] == juce::String("dashed"))
                {
                    float dash[] = { 4.0, 1.0 };
                    strokeType.createDashedStroke(strokedPath, p, dash, 2);
                }
                else
                    strokeType.createStrokedPath(strokedPath, p);
                g.fillPath(strokedPath);
            }

            // Path the clip region.
            juce::Path clip;
            for (int edgeNo = TOP; edgeNo <= LEFT; ++edgeNo)
            {
                float width = widths[edgeNo];
                if (width == 0)
                    continue;

                int prevEdgeNo = edgeNo == TOP ? LEFT : edgeNo - 1;
                int nextEdgeNo = edgeNo == LEFT ? TOP : edgeNo + 1;
                float prevWidth = widths[prevEdgeNo];
                float nextWidth = widths[nextEdgeNo];
                float startRadius = radii[edgeNo];
                float endRadius = radii[nextEdgeNo];

                juce::Path p = makeEdgePath(static_cast<BorderEdge>(edgeNo),
                                            width, 0,
                                            prevWidth, nextWidth,
                                            startRadius, endRadius,
                                            borderWidth, borderHeight);
                clip.addPath(p);
            }
            g.reduceClipRegion(clip);
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

        throw std::logic_error(std::string("Caller attempted to invoke a non-existent View method ") + method.toStdString());
    }
}
