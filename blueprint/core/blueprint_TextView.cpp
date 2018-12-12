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
        TextView* context = reinterpret_cast<TextView*>(YGNodeGetContext(node));

        return {
            context->getFont().getStringWidthFloat(context->getTextValue()),
            context->getFont().getHeight()
        };
    }

}
