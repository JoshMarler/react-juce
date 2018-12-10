/*
  ==============================================================================

    blueprint_View.h
    Created: 26 Nov 2018 3:38:37am

  ==============================================================================
*/

#pragma once

#include <map>


namespace blueprint
{

    //==============================================================================
    /** The View class is the core component abstraction for Blueprint's declarative
        flex-based component composition.
     */
    class View : public juce::Component
    {
    public:
        //==============================================================================
        View()
        {
            YGConfigSetUseWebDefaults(YGConfigGetDefault(), true);
            yogaNode = YGNodeNew();
        }

        virtual ~View()
        {
            YGNodeFree(yogaNode);
        }

        //==============================================================================
        /** Set a property on the nativ view. */
        void setProperty (const juce::Identifier& name, const juce::var& newValue);

        /** Adds a child component behind the existing children. */
        void appendChild (View* childView);

        /** Recalculates the layout tree, with this node as the root, inside the provided bounds. */
        void performLayout (juce::Rectangle<float> bounds);

        /** Asks this component to set its own bounds to those returned by getCachedFlexLayout(). */
        void updateLayout();

        /** Returns a juce::Rectangle describing the cached bounds of the internal Yoga node. */
        juce::Rectangle<float> getCachedFlexLayout();

        //==============================================================================
        void paint (juce::Graphics&) override;
        void resized() override;

    private:
        //==============================================================================
        YGNodeRef yogaNode;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (View)
    };

}
