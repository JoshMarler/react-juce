/*
  ==============================================================================

    ShadowView.h
    Created: 17 Apr 2019 8:38:37am

  ==============================================================================
*/

#pragma once

#include "View.h"


#define BP_SPREAD_SETTER_PERCENT(setter) setter, setter##Percent
#define BP_SPREAD_SETTER_AUTO(setter) BP_SPREAD_SETTER_PERCENT(setter), setter##Auto

namespace reactjuce
{

    struct BoundsAnimator : public juce::Timer {
        using StepCallback = std::function<void(juce::Rectangle<float>)>;

        enum class EasingType {
            Linear,
            QuadraticIn,
            QuadraticOut,
            QuadraticInOut,
        };

        juce::Rectangle<float> start;
        juce::Rectangle<float> dest;
        StepCallback callback;
        double duration;
        double startTime;
        int frameRate = 45;
        EasingType easingType = EasingType::Linear;

        BoundsAnimator(double durationMs, int frameRateToUse, EasingType et,
                       juce::Rectangle<float> startRect, juce::Rectangle<float> destRect,
                       StepCallback cb)
            : start(startRect)
            , dest(destRect)
            , callback(std::move (cb))
            , duration(durationMs)
            , frameRate(frameRateToUse)
            , easingType(et)
        {
            startTime = juce::Time::getMillisecondCounterHiRes();
            startTimerHz(45);
        }

        ~BoundsAnimator() override {
            stopTimer();
        }

        static constexpr float  lerp (float a, float b, double t)  { return a + (static_cast<float> (t) * (b - a)); }

        void timerCallback() override {
            auto now = juce::Time::getMillisecondCounterHiRes();
            double t = std::clamp((now - startTime) / duration, 0.0, 1.0);

            // Super helpful cheat sheet: https://gist.github.com/gre/1650294
            switch (easingType) {
                case EasingType::Linear:
                    break;
                case EasingType::QuadraticIn:
                    t = t * t;
                    break;
                case EasingType::QuadraticOut:
                    t = t * (2.0 - t);
                    break;
                case EasingType::QuadraticInOut:
                    t = (t < 0.5) ? (2.0 * t * t) : (-1.0 + (4.0 - 2.0 * t) * t);
                    break;
                default:
                    break;
            }

            if (t >= 0.9999) {
                callback(dest);
                stopTimer();
                return;
            }

            callback({
                lerp(start.getX(), dest.getX(), t),
                lerp(start.getY(), dest.getY(), t),
                lerp(start.getWidth(), dest.getWidth(), t),
                lerp(start.getHeight(), dest.getHeight(), t),
            });
        }
    };

    template <typename Setter, typename ...Args>
    const auto getYogaNodeFloatSetter(Setter setter, Args... args) {
      return [=](const juce::var& value, YGNodeRef node) {
        if(value.isDouble()) {
          setter(node, args..., (float) value);
          return true;
        }
        return false;
      };
    }

    template <typename Setter, typename SetterPercent, typename ...Args>
    const auto getYogaNodeDimensionSetter(Setter setter, SetterPercent setterPercent, Args... args) {
      return [=, floatSetter = getYogaNodeFloatSetter(setter, args...)](const juce::var& value, YGNodeRef node) {
        if (floatSetter(value, node))
          return true;
        if (value.isString() && value.toString().contains("%"))
        {
          juce::String strVal = value.toString().retainCharacters("-1234567890.");
          setterPercent(node, args..., strVal.getFloatValue());
          return true;
        }
        setter(node, args..., YGUndefined);
        return true;
      };
    }

    template <typename Setter, typename SetterPercent, typename SetterAuto, typename ...Args>
    const auto getYogaNodeDimensionAutoSetter(Setter setter, SetterPercent setterPercent, SetterAuto setterAuto, Args... args) {
      return [=, nonAutoSetter = getYogaNodeDimensionSetter(setter, setterPercent, args...)](const juce::var& value, YGNodeRef node) {
        if (value.isString() && value.toString() == "auto") {
          setterAuto(node, args...);
          return true;
        }
        return nonAutoSetter(value, node);
      };
    }

    template <typename Setter, typename EnumMap>
    const auto getYogaNodeEnumSetter(Setter setter, EnumMap &map) {
      return [=](const juce::var& value, YGNodeRef node) {                       \
        const auto val = map.find(value);
        if(val == map.end()) {
          // TODO catch further up to add the key at which we tried
          // to set this enum property to the message and rethrow
          throw std::invalid_argument("Invalid property: " + value.toString().toStdString());
        }
        setter(node, val->second);
        return true;
      };
    }

    //==============================================================================
    /** The ShadowView class decouples layout constraints from the actual View instances
        so that our View tree and ShadowView tree might differ (i.e. in the case of raw
        text nodes), and so that our View may remain more simple.
     */
    class ShadowView
    {
    public:
        //==============================================================================
        static const inline juce::Identifier debugProp          = "debug";
        static const inline juce::Identifier durationProp       = "duration";
        static const inline juce::Identifier easingProp         = "easing";
        static const inline juce::Identifier frameRateProp      = "frameRate";
        static const inline juce::Identifier layoutAnimatedProp = "layoutAnimated";

        //==============================================================================
        ShadowView(View* _view) : view(_view)
        {
            YGConfigSetUseWebDefaults(YGConfigGetDefault(), true);
            yogaNode = YGNodeNew();
        }

        virtual ~ShadowView()
        {
            YGNodeFree(yogaNode);
        }

        //==============================================================================
        /** Set a property on the shadow view. */
        virtual bool setProperty (const juce::String& name, const juce::var& newValue);

        /** Adds a child component behind the existing children. */
        virtual void addChild (ShadowView* childView, int index = -1)
        {
            if (index == -1)
            {
                YGNodeInsertChild(yogaNode, childView->yogaNode, YGNodeGetChildCount(yogaNode));
                children.push_back(childView);
            }
            else
            {
                jassert (juce::isPositiveAndNotGreaterThan(index, YGNodeGetChildCount(yogaNode)));

                YGNodeInsertChild(yogaNode, childView->yogaNode, static_cast<uint32_t> (index));
                children.insert(children.begin() + index, childView);
            }
        }

        /** Removes a child component from the children array. */
        virtual void removeChild (ShadowView* childView)
        {
            auto it = std::find(children.begin(), children.end(), childView);

            if (it != children.end())
            {
                YGNodeRemoveChild(yogaNode, childView->yogaNode);
                children.erase(it);
            }
        }

        //==============================================================================
        /** Returns a pointer to the View instance shadowed by this node. */
        View* getAssociatedView() { return view; }

        /** Returns the layout bounds held by the internal yogaNode. */
        juce::Rectangle<float> getCachedLayoutBounds()
        {
            return {
                YGNodeLayoutGetLeft(yogaNode),
                YGNodeLayoutGetTop(yogaNode),
                YGNodeLayoutGetWidth(yogaNode),
                YGNodeLayoutGetHeight(yogaNode)
            };
        }

        /** Recursively computes the shadow tree layout. */
        void computeViewLayout(const float width, const float height)
        {
            // Compute the new layout values
            YGNodeCalculateLayout(yogaNode, width, height, YGDirectionInherit);
        }

        /** Recursive traversal of the shadow tree, flushing layout bounds to
            the associated view components.
         */
        virtual void flushViewLayout()
        {
#ifdef DEBUG
            if (props.contains(debugProp))
                YGNodePrint(yogaNode, (YGPrintOptions) (YGPrintOptionsLayout
                                                        | YGPrintOptionsStyle
                                                        | YGPrintOptionsChildren));
#endif

            if (props.contains(layoutAnimatedProp))
            {
                if (props[layoutAnimatedProp].isBool() && props[layoutAnimatedProp])
                {
                    // Default parameters
                    return flushViewLayoutAnimated(50.0, 45, BoundsAnimator::EasingType::Linear);
                }

                if (props[layoutAnimatedProp].isObject())
                {
                    double const durationMs = props[layoutAnimatedProp].getProperty(durationProp, 50.0);
                    double const frameRate = props[layoutAnimatedProp].getProperty(frameRateProp, 45);
                    int const et = props[layoutAnimatedProp].getProperty(easingProp, 0);

                    return flushViewLayoutAnimated(durationMs, static_cast<int> (frameRate), static_cast<BoundsAnimator::EasingType>(et));
                }
            }

            view->setFloatBounds(getCachedLayoutBounds());
            view->setBounds(getCachedLayoutBounds().toNearestInt());

            for (auto& child : children)
                child->flushViewLayout();
        }

        /** Recursive traversal of the shadow tree, flushing layout bounds to the
         *  associated view components smoothly over time. This recursive step allows
         *  an animation of a component subtree by just marking the parent as animated.
         */
        virtual void flushViewLayoutAnimated(double const durationMs, int const frameRate, BoundsAnimator::EasingType const et)
        {
            auto viewCurrentBounds = view->getBounds().toFloat();
            auto viewDestinationBounds = getCachedLayoutBounds();

            animator = std::make_unique<BoundsAnimator>(
                durationMs,
                frameRate,
                et,
                viewCurrentBounds,
                viewDestinationBounds,
                [safeView = juce::Component::SafePointer(view)](auto && stepBounds) {
                    if (auto* v = safeView.getComponent()) {
                        v->setFloatBounds(stepBounds);
                        v->setBounds(stepBounds.toNearestInt());
                    }
                }
            );

            for (auto& child : children) {
                child->flushViewLayoutAnimated(durationMs, frameRate, et);
            }
        }

    protected:
        //==============================================================================
        YGNodeRef yogaNode;
        View* view = nullptr;
        juce::NamedValueSet props;

        std::unique_ptr<BoundsAnimator> animator;
        std::vector<ShadowView*> children;

    private:
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShadowView)
    };

}
