#pragma once

namespace blueprint
{
    /** The ScrollViewContentShadowView extends a ShadowView to provide specialized
        behaviour for flushing layout bounds to the content element of a ScrolLView.

        In particular, the default ShadowView behaviour doesn't understand that
        the child view of a ScrollView might be offset from the (0, 0) top left
        position. Because of that, when flushing layout bounds it will set bounds
        with a (0, 0) top left position, which the juce::Viewport understands as
        a command to scroll back to (0, 0). So here we just make sure to preserve
        any offset value when flushing new bounds.
    */
    class ScrollViewContentShadowView final : public ShadowView
    {
    public:
        //==============================================================================
        ScrollViewContentShadowView (View* _view) : ShadowView(_view) {}

        //==============================================================================
        void flushViewLayout() override
        {
            const auto bounds = getCachedLayoutBounds().withPosition (view->getPosition().toFloat());

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
