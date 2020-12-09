#pragma once

namespace blueprint
{
    /** The ScrollView class is a core view for scrollable content within Blueprint's
        layout system. It's basically a proxy component where the appendChild/removeChild
        methods delegate to a single child juce::Viewport.
    */
    class ScrollView final : public View
    {
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

        //==============================================================================
        /** */
        ScrollView();

        //==============================================================================
        /** @internal */
        void setProperty (const juce::Identifier& name, const juce::var& value) override;
        /** @internal */
        void addChild (View* childView, int index = -1) override;
        /** @internal */
        void resized() override;

    private:
        //==============================================================================
        juce::Viewport viewport;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScrollView)
    };

}
