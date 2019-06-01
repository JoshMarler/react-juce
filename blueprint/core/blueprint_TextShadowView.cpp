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

        // TODO: This is a bit of an oversimplification. We have a YGMeasureMode which
        // is one of three things, "undefined", "exact", or "at-most." Here we're kind of
        // just ignoring that, and in cases like `white-space: nowrap;` we want to ignore it,
        // but it would probably be good to get specific for each case.
        // See https://github.com/facebook/yoga/pull/576/files
        auto bounds = view->getGlyphArrangement(width).getBoundingBox(0, -1, true);

        return {
            bounds.getWidth(),
            bounds.getHeight()
        };
    }

}
