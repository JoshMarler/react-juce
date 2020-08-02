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
        struct FlexProperties
        {
            // Flex enums
            juce::Identifier direction       = "direction";
            juce::Identifier flexDirection   = "flex-direction";
            juce::Identifier justifyContent  = "justify-content";
            juce::Identifier alignItems      = "align-items";
            juce::Identifier alignContent    = "align-content";
            juce::Identifier alignSelf       = "align-self";
            juce::Identifier position        = "position";
            juce::Identifier flexWrap        = "flex-wrap";
            juce::Identifier overflow        = "overflow";

            // Flex dimensions
            juce::Identifier flex            = "flex";
            juce::Identifier flexGrow        = "flex-grow";
            juce::Identifier flexShrink      = "flex-shrink";
            juce::Identifier flexBasis       = "flex-basis";
            juce::Identifier width           = "width";
            juce::Identifier height          = "height";
            juce::Identifier minWidth        = "min-width";
            juce::Identifier minHeight       = "min-height";
            juce::Identifier maxWidth        = "max-width";
            juce::Identifier maxHeight       = "max-height";
            juce::Identifier aspectRatio     = "aspect-ratio";

            // Margin
            juce::Identifier marginMetaProp   = "margin";
            juce::Identifier marginLeft       = juce::String("margin-") + YGEdgeToString(YGEdgeLeft);
            juce::Identifier marginRight      = juce::String("margin-") + YGEdgeToString(YGEdgeRight);
            juce::Identifier marginTop        = juce::String("margin-") + YGEdgeToString(YGEdgeTop);
            juce::Identifier marginBottom     = juce::String("margin-") + YGEdgeToString(YGEdgeBottom);
            juce::Identifier marginStart      = juce::String("margin-") + YGEdgeToString(YGEdgeStart);
            juce::Identifier marginEnd        = juce::String("margin-") + YGEdgeToString(YGEdgeEnd);
            juce::Identifier marginHorizontal = juce::String("margin-") + YGEdgeToString(YGEdgeHorizontal);
            juce::Identifier marginVertical   = juce::String("margin-") + YGEdgeToString(YGEdgeVertical);

            // Padding
            juce::Identifier paddingMetaProp   = "padding";
            juce::Identifier paddingLeft       = juce::String("padding-") + YGEdgeToString(YGEdgeLeft);
            juce::Identifier paddingRight      = juce::String("padding-") + YGEdgeToString(YGEdgeRight);
            juce::Identifier paddingTop        = juce::String("padding-") + YGEdgeToString(YGEdgeTop);
            juce::Identifier paddingBottom     = juce::String("padding-") + YGEdgeToString(YGEdgeBottom);
            juce::Identifier paddingStart      = juce::String("padding-") + YGEdgeToString(YGEdgeStart);
            juce::Identifier paddingEnd        = juce::String("padding-") + YGEdgeToString(YGEdgeEnd);
            juce::Identifier paddingHorizontal = juce::String("padding-") + YGEdgeToString(YGEdgeHorizontal);
            juce::Identifier paddingVertical   = juce::String("padding-") + YGEdgeToString(YGEdgeVertical);

            //Position
            juce::Identifier positionLeft      = YGEdgeToString(YGEdgeLeft);
            juce::Identifier positionRight     = YGEdgeToString(YGEdgeRight);
            juce::Identifier positionTop       = YGEdgeToString(YGEdgeTop);
            juce::Identifier positionBottom    = YGEdgeToString(YGEdgeBottom);
            //TODO: Any need for other edge values? i.e. start, end ?
        } flexProperties;

        bool isMarginProp(const juce::Identifier& prop)
        {
            static const std::array<juce::Identifier, 9> marginProps =
            {
                flexProperties.marginMetaProp,
                flexProperties.marginLeft,
                flexProperties.marginRight,
                flexProperties.marginTop,
                flexProperties.marginBottom,
                flexProperties.marginStart,
                flexProperties.marginEnd,
                flexProperties.marginHorizontal,
                flexProperties.marginVertical
            };

            return std::find(marginProps.cbegin(), marginProps.cend(), prop) != marginProps.cend();
        }

        bool isPaddingProp(const juce::Identifier& prop)
        {
            static std::array<juce::Identifier, 9> paddingProps =
            {
                flexProperties.paddingMetaProp,
                flexProperties.paddingLeft,
                flexProperties.paddingRight,
                flexProperties.paddingTop,
                flexProperties.paddingBottom,
                flexProperties.paddingStart,
                flexProperties.paddingEnd,
                flexProperties.paddingHorizontal,
                flexProperties.paddingVertical
            };

            return std::find(paddingProps.cbegin(), paddingProps.cend(), prop) != paddingProps.cend();
        }

        bool isPositionProp(const juce::Identifier& prop)
        {
            static std::array<juce::Identifier, 4> positionProps =
            {
                flexProperties.positionLeft,
                flexProperties.positionRight,
                flexProperties.positionTop,
                flexProperties.positionBottom,
            };

            return std::find(positionProps.cbegin(), positionProps.cend(), prop) != positionProps.cend();
        }

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
            return std::any_of(validValues.cbegin(), validValues.cend(), [=] (const auto &pair)
            {
                return value.equalsIgnoreCase(pair.first);
            });
        }
    }

    //==============================================================================
    bool ShadowView::isLayoutProperty(const juce::Identifier& property)
    {
        static const std::array<juce::Identifier, 21> properties =
        {
            flexProperties.direction,
            flexProperties.flexDirection,
            flexProperties.justifyContent,
            flexProperties.alignItems,
            flexProperties.alignContent,
            flexProperties.alignSelf,
            flexProperties.position,
            flexProperties.flexWrap,
            flexProperties.overflow,
            flexProperties.flex,
            flexProperties.flexGrow,
            flexProperties.flexShrink,
            flexProperties.flexBasis,
            flexProperties.width,
            flexProperties.height,
            flexProperties.minWidth,
            flexProperties.minHeight,
            flexProperties.minWidth,
            flexProperties.maxWidth,
            flexProperties.maxHeight,
            flexProperties.aspectRatio
        };

        if (std::find(properties.cbegin(), properties.cend(), property) != properties.cend())
            return true;
        else if (isMarginProp(property))
            return true;
        else if (isPaddingProp(property))
            return true;
        else if (isPositionProp(property))
            return true;
        else
            return false;
    }

    //==============================================================================
    void ShadowView::setProperty (const juce::Identifier& name, const juce::var& newValue)
    {
        props.set(name, newValue);

        //==============================================================================
        // Flex enums
        if (name == flexProperties.direction)
        {
            jassert (validateFlexProperty(newValue, ValidDirectionValues));
            YGNodeStyleSetDirection(yogaNode, ValidDirectionValues[newValue]);
            return;
        }

        if (name == flexProperties.flexDirection)
        {
            jassert (validateFlexProperty(newValue, ValidFlexDirectionValues));
            YGNodeStyleSetFlexDirection(yogaNode, ValidFlexDirectionValues[newValue]);
            return;
        }

        if (name == flexProperties.justifyContent)
        {
            jassert (validateFlexProperty(newValue, ValidJustifyValues));
            YGNodeStyleSetJustifyContent(yogaNode, ValidJustifyValues[newValue]);
            return;
        }

        if (name == flexProperties.alignItems)
        {
            jassert (validateFlexProperty(newValue, ValidAlignValues));
            YGNodeStyleSetAlignItems(yogaNode, ValidAlignValues[newValue]);
            return;
        }

        if (name == flexProperties.alignContent)
        {
            jassert (validateFlexProperty(newValue, ValidAlignValues));
            YGNodeStyleSetAlignContent(yogaNode, ValidAlignValues[newValue]);
            return;
        }

        if (name == flexProperties.alignSelf)
        {
            jassert (validateFlexProperty(newValue, ValidAlignValues));
            YGNodeStyleSetAlignSelf(yogaNode, ValidAlignValues[newValue]);
            return;
        }

        if (name == flexProperties.position)
        {
            jassert (validateFlexProperty(newValue, ValidPositionTypeValues));
            YGNodeStyleSetPositionType(yogaNode, ValidPositionTypeValues[newValue]);
            return;
        }

        if (name == flexProperties.flexWrap)
        {
            jassert (validateFlexProperty(newValue, ValidFlexWrapValues));
            YGNodeStyleSetFlexWrap(yogaNode, ValidFlexWrapValues[newValue]);
            return;
        }

        if (name == flexProperties.overflow)
        {
            jassert (validateFlexProperty(newValue, ValidOverflowValues));
            YGNodeStyleSetOverflow(yogaNode, ValidOverflowValues[newValue]);
            return;
        }

        //==============================================================================
        // Flex dimensions
        if (name == flexProperties.flex)
        {
            BP_SET_FLEX_FLOAT_PROPERTY(newValue, YGNodeStyleSetFlex, yogaNode)
            return;
        }
        if (name == flexProperties.flexGrow)
        {
            BP_SET_FLEX_FLOAT_PROPERTY(newValue, YGNodeStyleSetFlexGrow, yogaNode)
            return;
        }
        if (name == flexProperties.flexShrink)
        {
            BP_SET_FLEX_FLOAT_PROPERTY(newValue, YGNodeStyleSetFlexShrink, yogaNode)
            return;
        }
        if (name == flexProperties.flexBasis)
        {
            BP_SET_FLEX_DIMENSION_PROPERTY_AUTO(newValue, YGNodeStyleSetFlexBasis, yogaNode)
            return;
        }
        if (name == flexProperties.width)
        {
            BP_SET_FLEX_DIMENSION_PROPERTY_AUTO(newValue, YGNodeStyleSetWidth, yogaNode)
            return;
        }
        if (name == flexProperties.height)
        {
            BP_SET_FLEX_DIMENSION_PROPERTY_AUTO(newValue, YGNodeStyleSetHeight, yogaNode)
            return;
        }
        if (name == flexProperties.minWidth)
        {
            BP_SET_FLEX_DIMENSION_PROPERTY(newValue, YGNodeStyleSetMinWidth, yogaNode)
            return;
        }
        if (name == flexProperties.minHeight)
        {
            BP_SET_FLEX_DIMENSION_PROPERTY(newValue, YGNodeStyleSetMinHeight, yogaNode)
            return;
        }
        if (name == flexProperties.maxWidth)
        {
            BP_SET_FLEX_DIMENSION_PROPERTY(newValue, YGNodeStyleSetMaxWidth, yogaNode)
            return;
        }
        if (name == flexProperties.maxHeight)
        {
            BP_SET_FLEX_DIMENSION_PROPERTY(newValue, YGNodeStyleSetMaxHeight, yogaNode)
            return;
        }
        if (name == flexProperties.aspectRatio)
        {
            BP_SET_FLEX_FLOAT_PROPERTY(newValue, YGNodeStyleSetAspectRatio, yogaNode)
            return;
        }

        //==============================================================================
        // Margin
        if (isMarginProp(name))
        {
            if (name == flexProperties.marginMetaProp)
                BP_SET_FLEX_DIMENSION_PROPERTY_AUTO(newValue, YGNodeStyleSetMargin, yogaNode, YGEdgeAll)
            else
                BP_SET_FLEX_DIMENSION_PROPERTY_AUTO(newValue, YGNodeStyleSetMargin, yogaNode, ValidEdgeValues[name.toString().replace("margin-", "")])

           return;
        }

        //==============================================================================
        // Padding
        if (isPaddingProp(name))
        {
            if (name == flexProperties.paddingMetaProp)
                BP_SET_FLEX_DIMENSION_PROPERTY(newValue, YGNodeStyleSetPadding, yogaNode, YGEdgeAll)
            else
                BP_SET_FLEX_DIMENSION_PROPERTY(newValue, YGNodeStyleSetMargin, yogaNode, ValidEdgeValues[name.toString().replace("padding-", "")])

            return;
        }

        //==============================================================================
        // Position
        if (isPositionProp(name))
        {
            BP_SET_FLEX_DIMENSION_PROPERTY(newValue, YGNodeStyleSetPosition, yogaNode, ValidEdgeValues[name.toString()]);
            return;
        }
    }
}
