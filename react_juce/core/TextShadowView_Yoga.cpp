#include "TextShadowView.h"
#include "YogaImplInclude.cpp"

namespace reactjuce
{
    //==============================================================================
    /** We use this method to measure the size of a given string so that the
     *  text container knows what size to take.
     */
    namespace
    {
        YGSize measureTextNode(YGNodeRef node, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode)
        {
            juce::ignoreUnused(widthMode);
            juce::ignoreUnused(height);
            juce::ignoreUnused(heightMode);

            auto context = reinterpret_cast<TextShadowView*>(YGNodeGetContext(node));
            auto view = dynamic_cast<TextView*>(context->getAssociatedView());

            jassert (view != nullptr);

            // TODO: This is a bit of an oversimplification. We have a YGMeasureMode which
            // is one of three things, "undefined", "exact", or "at-most." Here we're kind of
            // just ignoring that, and in cases like `white-space: nowrap;` we want to ignore it,
            // but it would probably be good to get specific for each case.
            // See https://github.com/facebook/yoga/pull/576/files
            auto tl = view->getTextLayout(width);

            return { tl.getWidth(), tl.getHeight() };
        }
    }
    
    //==============================================================================
    class TextShadowView::TextShadowViewPimpl
    {
    public:
        explicit TextShadowViewPimpl(TextShadowView &tv)
            : textShadowView(tv)
        {
            YGNodeSetContext(textShadowView.getShadowViewImpl().yogaNode, &textShadowView);
            YGNodeSetMeasureFunc(textShadowView.getShadowViewImpl().yogaNode, measureTextNode);
        }

        void markDirty()
        {
            YGNodeMarkDirty(textShadowView.getShadowViewImpl().yogaNode);
        }

    private:
        TextShadowView &textShadowView;
    };
    //==============================================================================

}
