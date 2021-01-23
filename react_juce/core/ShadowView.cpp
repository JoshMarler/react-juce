/*
  ==============================================================================

    ShadowView.cpp
    Created: 17 Apr 2019 10:07:17am

  ==============================================================================
*/

#include "ShadowView.h"


namespace reactjuce
{

    namespace
    {
        //==============================================================================
        std::unordered_map<juce::String, YGDirection> ValidDirectionValues {
            { YGDirectionToString(YGDirectionInherit), YGDirectionInherit },
            { YGDirectionToString(YGDirectionLTR), YGDirectionLTR },
            { YGDirectionToString(YGDirectionRTL), YGDirectionRTL },
        };

        std::unordered_map<juce::String, YGFlexDirection> ValidFlexDirectionValues {
            { YGFlexDirectionToString(YGFlexDirectionColumn), YGFlexDirectionColumn },
            { YGFlexDirectionToString(YGFlexDirectionColumnReverse), YGFlexDirectionColumnReverse },
            { YGFlexDirectionToString(YGFlexDirectionRow), YGFlexDirectionRow },
            { YGFlexDirectionToString(YGFlexDirectionRowReverse), YGFlexDirectionRowReverse },
        };

        std::unordered_map<juce::String, YGJustify> ValidJustifyValues {
            { YGJustifyToString(YGJustifyFlexStart), YGJustifyFlexStart },
            { YGJustifyToString(YGJustifyCenter), YGJustifyCenter },
            { YGJustifyToString(YGJustifyFlexEnd), YGJustifyFlexEnd },
            { YGJustifyToString(YGJustifySpaceBetween), YGJustifySpaceBetween },
            { YGJustifyToString(YGJustifySpaceAround), YGJustifySpaceAround },
        };

        std::unordered_map<juce::String, YGAlign> ValidAlignValues {
            { YGAlignToString(YGAlignAuto), YGAlignAuto },
            { YGAlignToString(YGAlignFlexStart), YGAlignFlexStart },
            { YGAlignToString(YGAlignCenter), YGAlignCenter },
            { YGAlignToString(YGAlignFlexEnd), YGAlignFlexEnd },
            { YGAlignToString(YGAlignStretch), YGAlignStretch },
            { YGAlignToString(YGAlignBaseline), YGAlignBaseline },
            { YGAlignToString(YGAlignSpaceBetween), YGAlignSpaceBetween },
            { YGAlignToString(YGAlignSpaceAround), YGAlignSpaceAround },
        };

        std::unordered_map<juce::String, YGPositionType> ValidPositionTypeValues {
            { YGPositionTypeToString(YGPositionTypeRelative), YGPositionTypeRelative },
            { YGPositionTypeToString(YGPositionTypeAbsolute), YGPositionTypeAbsolute },
        };

        std::unordered_map<juce::String, YGWrap> ValidFlexWrapValues {
            { YGWrapToString(YGWrapNoWrap), YGWrapNoWrap },
            { YGWrapToString(YGWrapWrap), YGWrapWrap },
            { YGWrapToString(YGWrapWrapReverse), YGWrapWrapReverse },
        };

        std::unordered_map<juce::String, YGOverflow> ValidOverflowValues {
            { YGOverflowToString(YGOverflowVisible), YGOverflowVisible },
            { YGOverflowToString(YGOverflowHidden), YGOverflowHidden },
            { YGOverflowToString(YGOverflowScroll), YGOverflowScroll },
        };


    //==============================================================================
    class PropertySetterMap {
      using K = juce::String;
      using V = juce::var;
      using F = std::function<bool(const V&, YGNodeRef)>;
      std::unordered_map<K, F> propertySetters;

      public:
        PropertySetterMap(std::initializer_list<std::pair<const K, F>> init): propertySetters(init) {}
        bool call(const K& key, const V& v, YGNodeRef node) const {
          const auto setter = propertySetters.find(key);
          if(setter == propertySetters.end()) {
            return false;
          }
          return setter->second(v, node);
        }
    };

    static const PropertySetterMap propertySetters{
        {"direction", getYogaNodeEnumSetter(YGNodeStyleSetDirection, ValidDirectionValues)},
        {"flex-direction", getYogaNodeEnumSetter(YGNodeStyleSetFlexDirection, ValidFlexDirectionValues)},
        {"justify-content", getYogaNodeEnumSetter(YGNodeStyleSetJustifyContent, ValidJustifyValues)},
        {"align-items", getYogaNodeEnumSetter(YGNodeStyleSetAlignItems, ValidAlignValues)},
        {"align-content", getYogaNodeEnumSetter(YGNodeStyleSetAlignContent, ValidAlignValues)},
        {"align-self", getYogaNodeEnumSetter(YGNodeStyleSetAlignSelf, ValidAlignValues)},
        {"position", getYogaNodeEnumSetter(YGNodeStyleSetPositionType, ValidPositionTypeValues)},
        {"flex-wrap", getYogaNodeEnumSetter(YGNodeStyleSetFlexWrap, ValidFlexWrapValues)},
        {"overflow", getYogaNodeEnumSetter(YGNodeStyleSetOverflow, ValidOverflowValues)},
        {"flex", getYogaNodeFloatSetter(YGNodeStyleSetFlex)},
        {"flex-grow", getYogaNodeFloatSetter(YGNodeStyleSetFlexGrow)},
        {"flex-shrink", getYogaNodeFloatSetter(YGNodeStyleSetFlexShrink)},
        {"flex-basis", getYogaNodeDimensionAutoSetter(BP_SPREAD_SETTER_AUTO(YGNodeStyleSetFlexBasis))},
        {"width", getYogaNodeDimensionAutoSetter(BP_SPREAD_SETTER_AUTO(YGNodeStyleSetWidth))},
        {"height", getYogaNodeDimensionAutoSetter(BP_SPREAD_SETTER_AUTO(YGNodeStyleSetHeight))},
        {"min-width", getYogaNodeDimensionSetter(BP_SPREAD_SETTER_PERCENT(YGNodeStyleSetMinWidth))},
        {"min-height", getYogaNodeDimensionSetter(BP_SPREAD_SETTER_PERCENT(YGNodeStyleSetMinHeight))},
        {"max-width", getYogaNodeDimensionSetter(BP_SPREAD_SETTER_PERCENT(YGNodeStyleSetMaxWidth))},
        {"max-height", getYogaNodeDimensionSetter(BP_SPREAD_SETTER_PERCENT(YGNodeStyleSetMaxHeight))},
        {"aspect-ratio", getYogaNodeFloatSetter(YGNodeStyleSetAspectRatio)},

        {"margin", getYogaNodeDimensionAutoSetter(BP_SPREAD_SETTER_AUTO(YGNodeStyleSetMargin), YGEdgeAll)},
        {juce::String("margin-") + YGEdgeToString(YGEdgeLeft), getYogaNodeDimensionAutoSetter(BP_SPREAD_SETTER_AUTO(YGNodeStyleSetMargin), YGEdgeLeft)},
        {juce::String("margin-") + YGEdgeToString(YGEdgeTop), getYogaNodeDimensionAutoSetter(BP_SPREAD_SETTER_AUTO(YGNodeStyleSetMargin), YGEdgeTop)},
        {juce::String("margin-") + YGEdgeToString(YGEdgeRight), getYogaNodeDimensionAutoSetter(BP_SPREAD_SETTER_AUTO(YGNodeStyleSetMargin), YGEdgeRight)},
        {juce::String("margin-") + YGEdgeToString(YGEdgeBottom), getYogaNodeDimensionAutoSetter(BP_SPREAD_SETTER_AUTO(YGNodeStyleSetMargin), YGEdgeBottom)},
        {juce::String("margin-") + YGEdgeToString(YGEdgeStart), getYogaNodeDimensionAutoSetter(BP_SPREAD_SETTER_AUTO(YGNodeStyleSetMargin), YGEdgeStart)},
        {juce::String("margin-") + YGEdgeToString(YGEdgeEnd), getYogaNodeDimensionAutoSetter(BP_SPREAD_SETTER_AUTO(YGNodeStyleSetMargin), YGEdgeEnd)},
        {juce::String("margin-") + YGEdgeToString(YGEdgeHorizontal), getYogaNodeDimensionAutoSetter(BP_SPREAD_SETTER_AUTO(YGNodeStyleSetMargin), YGEdgeHorizontal)},
        {juce::String("margin-") + YGEdgeToString(YGEdgeVertical), getYogaNodeDimensionAutoSetter(BP_SPREAD_SETTER_AUTO(YGNodeStyleSetMargin), YGEdgeLeft)},
        {juce::String("margin-") + YGEdgeToString(YGEdgeAll), getYogaNodeDimensionAutoSetter(BP_SPREAD_SETTER_AUTO(YGNodeStyleSetMargin), YGEdgeAll)},

        {"padding", getYogaNodeDimensionSetter(BP_SPREAD_SETTER_PERCENT(YGNodeStyleSetPadding), YGEdgeAll)},
        {juce::String("padding-") + YGEdgeToString(YGEdgeLeft), getYogaNodeDimensionSetter(BP_SPREAD_SETTER_PERCENT(YGNodeStyleSetPadding), YGEdgeLeft)},
        {juce::String("padding-") + YGEdgeToString(YGEdgeTop), getYogaNodeDimensionSetter(BP_SPREAD_SETTER_PERCENT(YGNodeStyleSetPadding), YGEdgeTop)},
        {juce::String("padding-") + YGEdgeToString(YGEdgeRight), getYogaNodeDimensionSetter(BP_SPREAD_SETTER_PERCENT(YGNodeStyleSetPadding), YGEdgeRight)},
        {juce::String("padding-") + YGEdgeToString(YGEdgeBottom), getYogaNodeDimensionSetter(BP_SPREAD_SETTER_PERCENT(YGNodeStyleSetPadding), YGEdgeBottom)},
        {juce::String("padding-") + YGEdgeToString(YGEdgeStart), getYogaNodeDimensionSetter(BP_SPREAD_SETTER_PERCENT(YGNodeStyleSetPadding), YGEdgeStart)},
        {juce::String("padding-") + YGEdgeToString(YGEdgeEnd), getYogaNodeDimensionSetter(BP_SPREAD_SETTER_PERCENT(YGNodeStyleSetPadding), YGEdgeEnd)},
        {juce::String("padding-") + YGEdgeToString(YGEdgeHorizontal), getYogaNodeDimensionSetter(BP_SPREAD_SETTER_PERCENT(YGNodeStyleSetPadding), YGEdgeHorizontal)},
        {juce::String("padding-") + YGEdgeToString(YGEdgeVertical), getYogaNodeDimensionSetter(BP_SPREAD_SETTER_PERCENT(YGNodeStyleSetPadding), YGEdgeLeft)},
        {juce::String("margin-") + YGEdgeToString(YGEdgeAll), getYogaNodeDimensionAutoSetter(BP_SPREAD_SETTER_AUTO(YGNodeStyleSetMargin), YGEdgeAll)},
        
        {YGEdgeToString(YGEdgeLeft), getYogaNodeDimensionSetter(BP_SPREAD_SETTER_PERCENT(YGNodeStyleSetPosition), YGEdgeLeft)},
        {YGEdgeToString(YGEdgeTop), getYogaNodeDimensionSetter(BP_SPREAD_SETTER_PERCENT(YGNodeStyleSetPosition), YGEdgeTop)},
        {YGEdgeToString(YGEdgeRight), getYogaNodeDimensionSetter(BP_SPREAD_SETTER_PERCENT(YGNodeStyleSetPosition), YGEdgeRight)},
        {YGEdgeToString(YGEdgeBottom), getYogaNodeDimensionSetter(BP_SPREAD_SETTER_PERCENT(YGNodeStyleSetPosition), YGEdgeBottom)},
    };
    }

    //==============================================================================
    bool ShadowView::setProperty (const juce::String& name, const juce::var& newValue)
    {
        props.set(name, newValue);

        return propertySetters.call(name, newValue, yogaNode);
    }
}
