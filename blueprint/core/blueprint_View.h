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
        virtual void setProperty (const juce::Identifier& name, const juce::var& newValue);

        /** Adds a child component behind the existing children. */
        void appendChild (View* childView);

        //==============================================================================
        /** Returns a juce::Rectangle describing the cached bounds of the internal Yoga node. */
        juce::Rectangle<float> getCachedFlexLayout();

        //==============================================================================
        void paint (juce::Graphics&) override;
        void resized() override;

    protected:
        //==============================================================================
        YGNodeRef yogaNode;

    private:
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (View)
    };

}
