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

#define BP_SET_FLEX_DIMENSION_PROPERTY_AUTO(setter, node, value)                    \
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
    BP_SET_YGVALUE_AUTO(ygval, setter, node);                                       \
}

#define BP_SET_FLEX_DIMENSION_PROPERTY(setter, node, value)                         \
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
    BP_SET_YGVALUE(ygval, setter, node);                                            \
}

#define BP_SET_FLEX_FLOAT_PROPERTY(setter, node, value) \
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
            BP_SET_FLEX_FLOAT_PROPERTY(YGNodeStyleSetFlex, yogaNode, newValue)
        if (name == juce::Identifier("flex-grow"))
            BP_SET_FLEX_FLOAT_PROPERTY(YGNodeStyleSetFlexGrow, yogaNode, newValue)
        if (name == juce::Identifier("flex-shrink"))
            BP_SET_FLEX_FLOAT_PROPERTY(YGNodeStyleSetFlexShrink, yogaNode, newValue)
        if (name == juce::Identifier("flex-basis"))
            BP_SET_FLEX_DIMENSION_PROPERTY_AUTO(YGNodeStyleSetFlexBasis, yogaNode, newValue)
        if (name == juce::Identifier("width"))
            BP_SET_FLEX_DIMENSION_PROPERTY_AUTO(YGNodeStyleSetWidth, yogaNode, newValue)
        if (name == juce::Identifier("height"))
            BP_SET_FLEX_DIMENSION_PROPERTY_AUTO(YGNodeStyleSetHeight, yogaNode, newValue)
        if (name == juce::Identifier("min-width"))
            BP_SET_FLEX_DIMENSION_PROPERTY(YGNodeStyleSetMinWidth, yogaNode, newValue)
        if (name == juce::Identifier("min-height"))
            BP_SET_FLEX_DIMENSION_PROPERTY(YGNodeStyleSetMinHeight, yogaNode, newValue)
        if (name == juce::Identifier("max-width"))
            BP_SET_FLEX_DIMENSION_PROPERTY(YGNodeStyleSetMaxWidth, yogaNode, newValue)
        if (name == juce::Identifier("max-height"))
            BP_SET_FLEX_DIMENSION_PROPERTY(YGNodeStyleSetMaxHeight, yogaNode, newValue)
        if (name == juce::Identifier("aspect-ratio"))
            BP_SET_FLEX_FLOAT_PROPERTY(YGNodeStyleSetAspectRatio, yogaNode, newValue)

        //==============================================================================
        // Flex position
        for (const auto& [edgeName, enumValue] : ValidEdgeValues)
        {
            if (name == juce::Identifier(edgeName))
            {
                YGNodeStyleSetPosition(yogaNode, enumValue, newValue);
                break;
            }
        }

        //==============================================================================
        // Margin & Padding
        juce::String prefix = name.toString().upToFirstOccurrenceOf("-", false, false);
        juce::String suffix = name.toString().fromLastOccurrenceOf("-", false, false);

        if (suffix.isEmpty() || prefix == suffix)
            suffix = "all";

        if (prefix == "margin")
        {
            for (const auto& [edgeName, enumValue] : ValidEdgeValues)
            {
                if (suffix == edgeName)
                {
                    YGNodeStyleSetMargin(yogaNode, enumValue, newValue);
                    break;
                }
            }
        }

        if (prefix == "padding")
        {
            for (const auto& [edgeName, enumValue] : ValidEdgeValues)
            {
                if (suffix == edgeName)
                {
                    YGNodeStyleSetPadding(yogaNode, enumValue, newValue);
                    break;
                }
            }
        }
    }

}
