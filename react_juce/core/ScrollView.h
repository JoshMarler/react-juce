/*
  ==============================================================================

    ScrollView.h
    Created: 20 Apr 2019 5:25:25pm

  ==============================================================================
*/

#pragma once

#include "View.h"


namespace reactjuce
{

    //==============================================================================
    /** The ScrollView class is a core view for scrollable content within Blueprint's
        layout system. It's basically a proxy component where the appendChild/removeChild
        methods delegate to a single child juce::Viewport.
     */
    class ScrollView : public View
                     , private juce::Timer
    {
        //==============================================================================
        class ScrollViewViewport : public juce::Viewport
        {
            using OnAreaChangedCallback = std::function<void(const juce::Rectangle<int>&)>;
        public:
            ScrollViewViewport() = default;

            void onAreaChanged(OnAreaChangedCallback callback)
            {
                onAreaChangedCallback = std::move(callback);
            }

            void visibleAreaChanged(const juce::Rectangle<int> &newArea) override
            {
                if (onAreaChangedCallback)
                    onAreaChangedCallback(newArea);
            }

        private:
             OnAreaChangedCallback onAreaChangedCallback;
        };

        //==============================================================================
        struct ScrollEvent
        {
            juce::var event = juce::var();
            bool      dirty = false;
        };

        //==============================================================================

    public:
        //==============================================================================
        // Props following CSS Scrollbars spec with some omissions/additions.
        // https://developer.mozilla.org/en-US/docs/Web/CSS/CSS_Scrollbars
        static inline juce::Identifier overflowXProp           = "overflow-x";
        static inline juce::Identifier overflowYProp           = "overflow-y";
        static inline juce::Identifier scrollBarWidthProp      = "scrollbar-width";
        static inline juce::Identifier scrollbarThumbColorProp = "scrollbar-thumb-color";
        static inline juce::Identifier scrollbarTrackColorProp = "scrollbar-track-color";
        static inline juce::Identifier scrollOnDragProp        = "scroll-on-drag";
        static inline juce::Identifier onScrollProp            = "onScroll";

        //==============================================================================
        ScrollView();

        //==============================================================================
        void setProperty (const juce::Identifier& name, const juce::var& value) override;
        void addChild (View* childView, int index = -1) override;

        //==============================================================================
        void resized() override;

    private:
        //==============================================================================
        void timerCallback() override;
        void exportNativeMethods();

        //==============================================================================
        ScrollViewViewport viewport;
        ScrollEvent        lastScrollEvent;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScrollView)
    };

}
