/*
  ==============================================================================

    blueprint_TextView.cpp
    Created: 28 Nov 2018 3:27:27pm

  ==============================================================================
*/

#pragma once


namespace blueprint
{

    //==============================================================================
    YGSize measureTextNode(YGNodeRef node, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) {
        TextShadowView* context = reinterpret_cast<TextShadowView*>(YGNodeGetContext(node));
        TextView* view = dynamic_cast<TextView*>(context->getAssociatedView());

        jassert (view != nullptr);

        auto bounds = view->getGlyphArrangement().getBoundingBox(0, -1, true);

        return {
            bounds.getWidth(),
            bounds.getHeight()
        };
    }

}
