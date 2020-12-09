#pragma once

namespace blueprint
{
    /** We use this method to measure the size of a given string so that the
        text container knows what size to take.
    */
    YGSize measureTextNode (YGNodeRef node,
                            float width, YGMeasureMode widthMode,
                            float height, YGMeasureMode heightMode);

    //==============================================================================
    /** The TextShadowView extends a ShadowView to provide specialized behaviour
        for measuring text content, as text layout is removed from the FlexBox flow.
    */
    class TextShadowView final : public ShadowView
    {
    public:
        //==============================================================================
        /** */
        TextShadowView(View* _view) : ShadowView(_view)
        {
            YGNodeSetContext(yogaNode, this);
            YGNodeSetMeasureFunc(yogaNode, measureTextNode);
        }

        //==============================================================================
        /** Sets a flag to indicate that this node needs to be measured at the next layout pass. */
        void markDirty()
        {
            YGNodeMarkDirty(yogaNode);
        }

        //==============================================================================
        /** @internal */
        bool setProperty (const juce::String& name, const juce::var& value) override
        {
            const bool layoutPropertyWasSet = ShadowView::setProperty(name, value);

            // For certain text properties we want Yoga to know that we need
            // to measure again. For example, changing the font size.
            if (name.compare("font-size") == 0)
                markDirty();

            return layoutPropertyWasSet;
        }

        /** @internal */
        void addChild (ShadowView* childView, int index = -1) override
        {
            juce::ignoreUnused (index);

            if (childView != nullptr)
            {
                jassertfalse;
                throw std::logic_error("TextShadowView cannot take children.");
            }
        }

    private:
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TextShadowView)
    };
}
