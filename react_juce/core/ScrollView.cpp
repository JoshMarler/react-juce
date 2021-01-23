/*
  ==============================================================================

    ScrollView.cpp
    Created: 10 Nov 2020 18:07pm

  ==============================================================================
*/

#include "ScrollView.h"


namespace reactjuce
{
    namespace detail
    {
        juce::var makeScrollEventObject(float scrollTopPosition, float scrollLeftPosition)
        {
            juce::DynamicObject::Ptr obj = new juce::DynamicObject();
            obj->setProperty("scrollTop", scrollTopPosition);
            obj->setProperty("scrollLeft", scrollLeftPosition);
            return obj.get();
        }
    }

    //==============================================================================
    ScrollView::ScrollView()
    {
        // Use a default scrollEvent rate of 30Hz.
        // This is prop configurable
        startTimerHz(30);

        viewport.onAreaChanged([this](const juce::Rectangle<int>& area)
        {
            lastScrollEvent.event = detail::makeScrollEventObject(area.getY(), area.getX());
            lastScrollEvent.dirty = true;
        });

        addAndMakeVisible(viewport);
        viewport.setScrollBarsShown(true, true);

        exportNativeMethods();
    }

    void ScrollView::setProperty (const juce::Identifier& name, const juce::var& value)
    {
        View::setProperty(name, value);

        auto showX = viewport.isHorizontalScrollBarShown();
        auto showY = viewport.isVerticalScrollBarShown();

        auto overflowCheck = [](const juce::String& p, const juce::String &v)
        {
            if (juce::StringRef("scroll") == v) { return true; }
            if (juce::StringRef("hidden") == v) { return false; }

            const juce::String e = "Invalid prop value. Prop '" + p + "' must be a string of 'hidden' or 'scroll'";
            throw std::invalid_argument(e.toStdString());
        };

        if (name == overflowYProp)
        {
            showY = overflowCheck(overflowYProp.toString(), value);
        }

        if (name == overflowXProp)
        {
            showX = overflowCheck(overflowXProp.toString(), value);
        }

        viewport.setScrollBarsShown(showY, showX);

        if (name == scrollBarWidthProp)
        {
            if (!props[scrollBarWidthProp].isDouble())
                throw std::invalid_argument("Invalid prop value. Prop \'scollbar-width\' must be a number.");

            const int thickness = props[scrollBarWidthProp];
            viewport.setScrollBarThickness(thickness);
        }

        if (name == scrollbarThumbColorProp)
        {
            if (!props[scrollbarThumbColorProp].isString())
                throw std::invalid_argument("Invalid prop value. Prop \'scrollbar-thumb-color\' must be a color string.");

            const auto thumbColor = juce::Colour::fromString(props[scrollbarThumbColorProp].toString());
            viewport.getVerticalScrollBar().setColour(juce::ScrollBar::thumbColourId, thumbColor);
            viewport.getHorizontalScrollBar().setColour(juce::ScrollBar::thumbColourId, thumbColor);
        }

        if (name == scrollbarTrackColorProp)
        {
            if (!props[scrollbarTrackColorProp].isString())
                throw std::invalid_argument("Invalid prop value. Prop \'scrollbar-track-color\' must be a color string.");

            const auto trackColor = juce::Colour::fromString(props[scrollbarTrackColorProp].toString());
            viewport.getVerticalScrollBar().setColour(juce::ScrollBar::trackColourId, trackColor);
            viewport.getHorizontalScrollBar().setColour(juce::ScrollBar::trackColourId, trackColor);
        }

        if (name == scrollOnDragProp)
        {
            if (!props[scrollOnDragProp].isBool())
                throw std::invalid_argument("Invalid prop value. Prop \'scroll-on-drag\' must be a bool.");

            const bool scrollOnDrag = props[scrollOnDragProp];
            viewport.setScrollOnDragEnabled(scrollOnDrag);
        }
    }

    void ScrollView::addChild (View* childView, int index)
    {
        juce::ignoreUnused (index);
        jassert (viewport.getViewedComponent() == nullptr);
        viewport.setViewedComponent(childView, false);
    }

    void ScrollView::resized()
    {
        const auto &bounds = getLocalBounds();
        viewport.setBounds(bounds);
        View::resized();
    }

    void ScrollView::timerCallback()
    {
        if (lastScrollEvent.dirty)
        {
            if (props.contains(onScrollProp) && props[onScrollProp].isMethod())
            {
                std::array<juce::var, 1> args { lastScrollEvent.event };
                juce::var::NativeFunctionArgs nfArgs(juce::var(), args.data(), static_cast<int>(args.size()));

                std::invoke(props[onScrollProp].getNativeFunction(), nfArgs);

                // Update lastScrollEvent dirty flag so it isn't raised again
                lastScrollEvent.dirty = false;
            }
        }
    }

    void ScrollView::exportNativeMethods()
    {
       exportMethod("scrollToPosition", [this] (const juce::var::NativeFunctionArgs &args) -> juce::var
       {
           jassert(args.numArguments == 2);
           const int xPos = args.arguments[0];
           const int yPos = args.arguments[1];

           viewport.setViewPosition(xPos, yPos);
           return juce::var::undefined();
       });
    }
}
