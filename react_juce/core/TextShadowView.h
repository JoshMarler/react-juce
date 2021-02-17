#pragma once

#include "ShadowView.h"
#include "View.h"


namespace reactjuce
{

    /** The TextShadowView extends a ShadowView to provide specialized behavior
     *  for measuring text content, as text layout is removed from the FlexBox
     *  flow.
     */
    class TextShadowView : public ShadowView
    {
    public:
        //==============================================================================
        explicit TextShadowView(View* _view);

        //==============================================================================
        /** Set a property on the shadow view. */
        bool setProperty (const juce::String& name, const juce::var& value) override;

        /** Override the default ShadowView behavior to explicitly error. */
        void addChild (ShadowView* childView, int index) override;

        //==============================================================================
        /** Sets a flag to indicate that this node needs to be measured at the next layout pass. */
        void markDirty();

        //==============================================================================

    private:
        //==============================================================================
        class TextShadowViewPimpl;
        std::unique_ptr<TextShadowViewPimpl> textShadowViewPimpl;

        friend TextShadowViewPimpl;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TextShadowView)
    };

}
