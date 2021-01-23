/*
  ==============================================================================

    ScrollViewContentShadowView.h
    Created: 16 Jun 2019 10:38:37pm

  ==============================================================================
*/

#pragma once

#include "ShadowView.h"
#include "View.h"


namespace reactjuce
{

    //==============================================================================
    /** The ScrollViewContentShadowView extends a ShadowView to provide specialized
     *  behavior for flushing layout bounds to the content element of a ScrolLView.
     *
     *  In particular, the default ShadowView behavior doesn't understand that
     *  the child view of a ScrollView might be offset from the (0, 0) top left
     *  position. Because of that, when flushing layout bounds it will set bounds
     *  with a (0, 0) top left position, which the juce::Viewport understands as
     *  a command to scroll back to (0, 0). So here we just make sure to preserve
     *  any offset value when flushing new bounds.
     */
    class ScrollViewContentShadowView : public ShadowView
    {
    public:
        //==============================================================================
        ScrollViewContentShadowView(View* _view)
            : ShadowView(_view) {}

        //==============================================================================
        void flushViewLayout() override
        {
            auto pos = view->getPosition().toFloat();
            auto bounds = getCachedLayoutBounds().withPosition(pos);

            view->setFloatBounds(bounds);
            view->setBounds(bounds.toNearestInt());

            for (auto& child : children)
                child->flushViewLayout();
        }

    private:
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScrollViewContentShadowView)
    };

}
