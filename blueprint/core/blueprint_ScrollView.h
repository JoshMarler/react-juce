/*
  ==============================================================================

    blueprint_ScrollView.h
    Created: 20 Apr 2019 5:25:25pm

  ==============================================================================
*/

#pragma once

#include "blueprint_View.h"


namespace blueprint
{

    //==============================================================================
    /** The ScrollView class is a core view for scrollable content within Blueprint's
        layout system. It's basically a proxy component where the appendChild/removeChild
        methods delegate to a single child juce::Viewport.
     */
    class ScrollView : public View
    {
    public:
        //==============================================================================
        ScrollView()
        {
            addAndMakeVisible(viewport);
            // TODO: Set these based on props...
            viewport.setScrollBarsShown(true, false);
        }

        //==============================================================================
        void setProperty (const juce::Identifier& name, const juce::var& value) override
        {
            View::setProperty(name, value);

            if (name == juce::Identifier("scrollbar-thumb-color"))
            {
                juce::Colour c = juce::Colour::fromString(props["scrollbar-thumb-color"].toString());

                viewport.getVerticalScrollBar().setColour(juce::ScrollBar::thumbColourId, c);
                viewport.getHorizontalScrollBar().setColour(juce::ScrollBar::thumbColourId, c);
            }
        }

        void addChild (View* childView, int index = -1) override
        {
            jassert (viewport.getViewedComponent() == nullptr);
            viewport.setViewedComponent(childView, false);
        }

        //==============================================================================
        void resized() override
        {
            viewport.setBounds(getLocalBounds());
            View::resized();
        }

    private:
        //==============================================================================
        juce::Viewport viewport;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScrollView)
    };

}
