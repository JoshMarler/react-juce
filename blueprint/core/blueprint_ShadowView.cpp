/*
  ==============================================================================

    blueprint_ShadowView.cpp
    Created: 17 Apr 2019 10:07:17am

  ==============================================================================
*/


#define BP_SET_YGVALUE(ygvalue, setter, ...)            \
switch (ygvalue.unit)                                   \
{                                                       \
    case YGUnitAuto:                                    \
    case YGUnitUndefined:                               \
        setter(__VA_ARGS__, YGUndefined);               \
        break;                                          \
    case YGUnitPoint:                                   \
        setter(__VA_ARGS__, ygvalue.value);             \
        break;                                          \
    case YGUnitPercent:                                 \
        setter##Percent(__VA_ARGS__, ygvalue.value);    \
        break;                                          \
}

#define BP_SET_YGVALUE_AUTO(ygvalue, setter, ...)       \
switch (ygvalue.unit)                                   \
{                                                       \
    case YGUnitAuto:                                    \
        setter##Auto(__VA_ARGS__);                      \
        break;                                          \
    case YGUnitUndefined:                               \
        setter(__VA_ARGS__, YGUndefined);               \
        break;                                          \
    case YGUnitPoint:                                   \
        setter(__VA_ARGS__, ygvalue.value);             \
        break;                                          \
    case YGUnitPercent:                                 \
        setter##Percent(__VA_ARGS__, ygvalue.value);    \
        break;                                          \
}

#define BP_SET_FLEX_DIMENSION_PROPERTY_AUTO(value, setter, ...)                     \
{                                                                                   \
    YGValue ygval = { 0.0f, YGUnitUndefined };                                      \
                                                                                    \
    if (value.isDouble())                                                           \
        ygval = { (float) value, YGUnitPoint };                                     \
    else if (value.isString() && value.toString() == "auto")                        \
        ygval = { 0.0f, YGUnitAuto };                                               \
    else if (value.isString() && value.toString().trim().contains("%"))             \
    {                                                                               \
        juce::String strVal = value.toString().retainCharacters("-1234567890.");    \
        ygval = { strVal.getFloatValue(), YGUnitPercent };                          \
    }                                                                               \
                                                                                    \
    BP_SET_YGVALUE_AUTO(ygval, setter, __VA_ARGS__);                                \
}

#define BP_SET_FLEX_DIMENSION_PROPERTY(value, setter, ...)                          \
{                                                                                   \
    YGValue ygval = { 0.0f, YGUnitUndefined };                                      \
                                                                                    \
    if (value.isDouble())                                                           \
        ygval = { (float) value, YGUnitPoint };                                     \
    else if (value.isString() && value.toString().trim().contains("%"))             \
    {                                                                               \
        juce::String strVal = value.toString().retainCharacters("-1234567890.");    \
        ygval = { strVal.getFloatValue(), YGUnitPercent };                          \
    }                                                                               \
                                                                                    \
    BP_SET_YGVALUE(ygval, setter, __VA_ARGS__);                                     \
}

#define BP_SET_FLEX_FLOAT_PROPERTY(value, setter, node) \
{                                                       \
    if (value.isDouble())                               \
        setter(node, (float) value);                    \
}

namespace blueprint
{

    namespace
    {

        //==============================================================================
        std::map<juce::String, YGDirection> ValidDirectionValues {
            { YGDirectionToString(YGDirectionInherit), YGDirectionInherit },
            { YGDirectionToString(YGDirectionLTR), YGDirectionLTR },
            { YGDirectionToString(YGDirectionRTL), YGDirectionRTL },
        };

        std::map<juce::String, YGFlexDirection> ValidFlexDirectionValues {
            { YGFlexDirectionToString(YGFlexDirectionColumn), YGFlexDirectionColumn },
            { YGFlexDirectionToString(YGFlexDirectionColumnReverse), YGFlexDirectionColumnReverse },
            { YGFlexDirectionToString(YGFlexDirectionRow), YGFlexDirectionRow },
            { YGFlexDirectionToString(YGFlexDirectionRowReverse), YGFlexDirectionRowReverse },
        };

        std::map<juce::String, YGJustify> ValidJustifyValues {
            { YGJustifyToString(YGJustifyFlexStart), YGJustifyFlexStart },
            { YGJustifyToString(YGJustifyCenter), YGJustifyCenter },
            { YGJustifyToString(YGJustifyFlexEnd), YGJustifyFlexEnd },
            { YGJustifyToString(YGJustifySpaceBetween), YGJustifySpaceBetween },
            { YGJustifyToString(YGJustifySpaceAround), YGJustifySpaceAround },
        };

        std::map<juce::String, YGAlign> ValidAlignValues {
            { YGAlignToString(YGAlignAuto), YGAlignAuto },
            { YGAlignToString(YGAlignFlexStart), YGAlignFlexStart },
            { YGAlignToString(YGAlignCenter), YGAlignCenter },
            { YGAlignToString(YGAlignFlexEnd), YGAlignFlexEnd },
            { YGAlignToString(YGAlignStretch), YGAlignStretch },
            { YGAlignToString(YGAlignBaseline), YGAlignBaseline },
            { YGAlignToString(YGAlignSpaceBetween), YGAlignSpaceBetween },
            { YGAlignToString(YGAlignSpaceAround), YGAlignSpaceAround },
        };

        std::map<juce::String, YGPositionType> ValidPositionTypeValues {
            { YGPositionTypeToString(YGPositionTypeRelative), YGPositionTypeRelative },
            { YGPositionTypeToString(YGPositionTypeAbsolute), YGPositionTypeAbsolute },
        };

        std::map<juce::String, YGWrap> ValidFlexWrapValues {
            { YGWrapToString(YGWrapNoWrap), YGWrapNoWrap },
            { YGWrapToString(YGWrapWrap), YGWrapWrap },
            { YGWrapToString(YGWrapWrapReverse), YGWrapWrapReverse },
        };

        std::map<juce::String, YGOverflow> ValidOverflowValues {
            { YGOverflowToString(YGOverflowVisible), YGOverflowVisible },
            { YGOverflowToString(YGOverflowHidden), YGOverflowHidden },
            { YGOverflowToString(YGOverflowScroll), YGOverflowScroll },
        };

        std::map<juce::String, YGEdge> ValidEdgeValues {
            { YGEdgeToString(YGEdgeLeft), YGEdgeLeft },
            { YGEdgeToString(YGEdgeTop), YGEdgeTop },
            { YGEdgeToString(YGEdgeRight), YGEdgeRight },
            { YGEdgeToString(YGEdgeBottom), YGEdgeBottom },
            { YGEdgeToString(YGEdgeStart), YGEdgeStart },
            { YGEdgeToString(YGEdgeEnd), YGEdgeEnd },
            { YGEdgeToString(YGEdgeHorizontal), YGEdgeHorizontal },
            { YGEdgeToString(YGEdgeVertical), YGEdgeVertical },
            { YGEdgeToString(YGEdgeAll), YGEdgeAll },
        };

        //==============================================================================
        template<typename T>
        bool validateFlexProperty (juce::String value, std::map<juce::String, T> validValues)
        {
            for (const auto& [flexValue, enumValue] : validValues)
            {
                if (value.equalsIgnoreCase(flexValue))
                {
                    return true;
                }
            }

            return false;
        }

    }

    //==============================================================================
    void ShadowView::setProperty (const juce::Identifier& name, const juce::var& newValue)
    {
        props.set(name, newValue);

        //==============================================================================
        // Flex enums
        if (name == juce::Identifier("direction"))
        {
            jassert (validateFlexProperty(newValue, ValidDirectionValues));
            YGNodeStyleSetDirection(yogaNode, ValidDirectionValues[newValue]);
        }

        if (name == juce::Identifier("flex-direction"))
        {
            jassert (validateFlexProperty(newValue, ValidFlexDirectionValues));
            YGNodeStyleSetFlexDirection(yogaNode, ValidFlexDirectionValues[newValue]);
        }

        if (name == juce::Identifier("justify-content"))
        {
            jassert (validateFlexProperty(newValue, ValidJustifyValues));
            YGNodeStyleSetJustifyContent(yogaNode, ValidJustifyValues[newValue]);
        }

        if (name == juce::Identifier("align-items"))
        {
            jassert (validateFlexProperty(newValue, ValidAlignValues));
            YGNodeStyleSetAlignItems(yogaNode, ValidAlignValues[newValue]);
        }

        if (name == juce::Identifier("align-content"))
        {
            jassert (validateFlexProperty(newValue, ValidAlignValues));
            YGNodeStyleSetAlignContent(yogaNode, ValidAlignValues[newValue]);
        }

        if (name == juce::Identifier("align-self"))
        {
            jassert (validateFlexProperty(newValue, ValidAlignValues));
            YGNodeStyleSetAlignSelf(yogaNode, ValidAlignValues[newValue]);
        }

        if (name == juce::Identifier("position"))
        {
            jassert (validateFlexProperty(newValue, ValidPositionTypeValues));
            YGNodeStyleSetPositionType(yogaNode, ValidPositionTypeValues[newValue]);
        }

        if (name == juce::Identifier("flex-wrap"))
        {
            jassert (validateFlexProperty(newValue, ValidFlexWrapValues));
            YGNodeStyleSetFlexWrap(yogaNode, ValidFlexWrapValues[newValue]);
        }

        if (name == juce::Identifier("overflow"))
        {
            jassert (validateFlexProperty(newValue, ValidOverflowValues));
            YGNodeStyleSetOverflow(yogaNode, ValidOverflowValues[newValue]);
        }

        //==============================================================================
        // Flex dimensions
        if (name == juce::Identifier("flex"))
            BP_SET_FLEX_FLOAT_PROPERTY(newValue, YGNodeStyleSetFlex, yogaNode)
        if (name == juce::Identifier("flex-grow"))
            BP_SET_FLEX_FLOAT_PROPERTY(newValue, YGNodeStyleSetFlexGrow, yogaNode)
        if (name == juce::Identifier("flex-shrink"))
            BP_SET_FLEX_FLOAT_PROPERTY(newValue, YGNodeStyleSetFlexShrink, yogaNode)
        if (name == juce::Identifier("flex-basis"))
            BP_SET_FLEX_DIMENSION_PROPERTY_AUTO(newValue, YGNodeStyleSetFlexBasis, yogaNode)
        if (name == juce::Identifier("width"))
            BP_SET_FLEX_DIMENSION_PROPERTY_AUTO(newValue, YGNodeStyleSetWidth, yogaNode)
        if (name == juce::Identifier("height"))
            BP_SET_FLEX_DIMENSION_PROPERTY_AUTO(newValue, YGNodeStyleSetHeight, yogaNode)
        if (name == juce::Identifier("min-width"))
            BP_SET_FLEX_DIMENSION_PROPERTY(newValue, YGNodeStyleSetMinWidth, yogaNode)
        if (name == juce::Identifier("min-height"))
            BP_SET_FLEX_DIMENSION_PROPERTY(newValue, YGNodeStyleSetMinHeight, yogaNode)
        if (name == juce::Identifier("max-width"))
            BP_SET_FLEX_DIMENSION_PROPERTY(newValue, YGNodeStyleSetMaxWidth, yogaNode)
        if (name == juce::Identifier("max-height"))
            BP_SET_FLEX_DIMENSION_PROPERTY(newValue, YGNodeStyleSetMaxHeight, yogaNode)
        if (name == juce::Identifier("aspect-ratio"))
            BP_SET_FLEX_FLOAT_PROPERTY(newValue, YGNodeStyleSetAspectRatio, yogaNode)

        //==============================================================================
        // Margin
        juce::Identifier marginMetaProp ("margin");

        for (const auto& [edgeName, enumValue] : ValidEdgeValues)
        {
            juce::Identifier propId (juce::String("margin-") + edgeName);

            if (name == propId || (name == marginMetaProp && enumValue == YGEdgeAll))
            {
                BP_SET_FLEX_DIMENSION_PROPERTY_AUTO(newValue, YGNodeStyleSetMargin, yogaNode, enumValue);
            }
        }

        //==============================================================================
        // Padding
        juce::Identifier paddingMetaProp ("padding");

        for (const auto& [edgeName, enumValue] : ValidEdgeValues)
        {
            juce::Identifier propId (juce::String("padding-") + edgeName);

            if (name == propId || (name == paddingMetaProp && enumValue == YGEdgeAll))
            {
                BP_SET_FLEX_DIMENSION_PROPERTY(newValue, YGNodeStyleSetPadding, yogaNode, enumValue);
            }
        }

        //==============================================================================
        // Position
        for (const auto& [edgeName, enumValue] : ValidEdgeValues)
        {
            if (name == juce::Identifier(edgeName))
            {
                BP_SET_FLEX_DIMENSION_PROPERTY(newValue, YGNodeStyleSetPosition, yogaNode, enumValue);
            }
        }
    }

}
