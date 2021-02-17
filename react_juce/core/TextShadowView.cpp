#include "TextShadowView.h"

//TODO: Eventually we may switch this out based on some preprocessor
//      flag just as we're doing with EcmascriptEngine.
#include "TextShadowView_Yoga.cpp"


namespace reactjuce
{

    //==============================================================================
    TextShadowView::TextShadowView(View* _view)
        : ShadowView(_view)
    {
        textShadowViewPimpl = std::make_unique<TextShadowViewPimpl>(*this);
    }

    //==============================================================================
    bool TextShadowView::setProperty (const juce::String& name, const juce::var& value)
    {
        const bool layoutPropertyWasSet = ShadowView::setProperty(name, value);

        // For certain text properties we want Yoga to know that we need
        // to measure again. For example, changing the font size.
        if (name.compare("font-size") == 0)
            markDirty();

        return layoutPropertyWasSet;
    }

    //==============================================================================
    void TextShadowView::addChild (ShadowView* childView, int index)
    {
        juce::ignoreUnused (index);
        if (childView != nullptr)
        {
            throw std::logic_error("TextShadowView cannot take children.");
        }
    }
    //==============================================================================
    void TextShadowView::markDirty()
    {
        textShadowViewPimpl->markDirty();
    }

    //==============================================================================
}
