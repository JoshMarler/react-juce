/*
  ==============================================================================

    blueprint_ScrollView.cpp
    Created: 10 Nov 2020 18:07pm

  ==============================================================================
*/

#include "blueprint_ScrollView.h"

namespace blueprint
{
    ScrollView::ScrollView()
    {
        addAndMakeVisible(viewport);
        viewport.setScrollBarsShown(true, true);
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
                throw std::invalid_argument("Invalid prop value. Prop \'scoll-on-drag\' must be a bool.");

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
}
