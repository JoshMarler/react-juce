/*
  ==============================================================================

    TextShadowView.h
    Created: 17 Apr 2019 8:38:37am

  ==============================================================================
*/

#pragma once

#include "ShadowView.h"
#include "View.h"


namespace reactjuce
{

    //==============================================================================
    /** We use this method to measure the size of a given string so that the
     *  text container knows what size to take.
     */
    YGSize measureTextNode(YGNodeRef, float, YGMeasureMode, float, YGMeasureMode);

    //==============================================================================
    /** The TextShadowView extends a ShadowView to provide specialized behavior
     *  for measuring text content, as text layout is removed from the FlexBox
     *  flow.
     */
    class TextShadowView : public ShadowView
    {
    public:
        //==============================================================================
        TextShadowView(View* _view) : ShadowView(_view)
        {
            YGNodeSetContext(yogaNode, this);
            YGNodeSetMeasureFunc(yogaNode, measureTextNode);
        }

        //==============================================================================
        /** Set a property on the shadow view. */
        bool setProperty (const juce::String& name, const juce::var& value) override
        {
            const bool layoutPropertyWasSet = ShadowView::setProperty(name, value);

            // For certain text properties we want Yoga to know that we need
            // to measure again. For example, changing the font size.
            if (name.compare("font-size") == 0)
                markDirty();

            return layoutPropertyWasSet;
        }

        /** Override the default ShadowView behavior to explicitly error. */
        void addChild (ShadowView* childView, int index = -1) override
        {
            juce::ignoreUnused (index);

            if (childView != nullptr)
            {
                throw std::logic_error("TextShadowView cannot take children.");
            }
        }

        /** Sets a flag to indicate that this node needs to be measured at the next layout pass. */
        void markDirty()
        {
            YGNodeMarkDirty(yogaNode);
        }

    private:
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TextShadowView)
    };

}
