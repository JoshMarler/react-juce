#pragma once

#define BP_SPREAD_SETTER_PERCENT(setter) setter, setter##Percent
#define BP_SPREAD_SETTER_AUTO(setter) BP_SPREAD_SETTER_PERCENT(setter), setter##Auto

namespace blueprint
{
    /** */
    struct BoundsAnimator final : public juce::Timer
    {
        //==============================================================================
        /** */
        using StepCallback = std::function<void (juce::Rectangle<float>)>;

        //==============================================================================
        /** */
        enum class EasingType
        {
            linear,
            quadraticIn,
            quadraticOut,
            quadraticInOut,
        };

        //==============================================================================
        /** */
        BoundsAnimator (double durationMs, int frameRateToUse, EasingType et,
                        juce::Rectangle<float> startRect, juce::Rectangle<float> destRect,
                        StepCallback cb) :
            start (startRect),
            dest (destRect),
            callback (std::move (cb)),
            duration (durationMs),
            frameRate (frameRateToUse),
            easingType (et)
        {
            startTime = juce::Time::getMillisecondCounterHiRes();
            startTimerHz (45);
        }

        //==============================================================================
        void timerCallback() override
        {
            const auto now = juce::Time::getMillisecondCounterHiRes();
            auto t = std::clamp ((now - startTime) / duration, 0.0, 1.0);

            // Super helpful cheat sheet: https://gist.github.com/gre/1650294
            switch (easingType)
            {
                case EasingType::quadraticIn:       t = juce::square (t); break;
                case EasingType::quadraticOut:      t = t * (2.0 - t); break;
                case EasingType::quadraticInOut:    t = (t < 0.5) ? (2.0 * t * t) : (-1.0 + (4.0 - 2.0 * t) * t); break;

                case EasingType::linear:
                default:
                    break;
            }

            if (t >= 0.9999)
            {
                callback (dest);
                stopTimer();
                return;
            }

            callback
            ({
                juce::jmap (static_cast<float> (t), start.getX(), dest.getX()),
                juce::jmap (static_cast<float> (t), start.getY(), dest.getY()),
                juce::jmap (static_cast<float> (t), start.getWidth(), dest.getWidth()),
                juce::jmap (static_cast<float> (t), start.getHeight(), dest.getHeight()),
            });
        }

        juce::Rectangle<float> start, dest;
        StepCallback callback;
        double duration = 0.0, startTime = 0.0;
        int frameRate = 45;
        EasingType easingType = EasingType::linear;
    };

    //==============================================================================
    template <typename Setter, typename ...Args>
    const auto getYogaNodeFloatSetter (Setter setter, Args... args)
    {
        return [=] (const juce::var& value, YGNodeRef node)
        {
            if (value.isDouble())
            {
                setter (node, args..., (float) value);
                return true;
            }

            return false;
        };
    }

    template <typename Setter, typename SetterPercent, typename ...Args>
    const auto getYogaNodeDimensionSetter (Setter setter, SetterPercent setterPercent, Args... args)
    {
        return [=, floatSetter = getYogaNodeFloatSetter(setter, args...)] (const juce::var& value, YGNodeRef node)
        {
            if (floatSetter (value, node))
                return true;

            if (value.isString() && value.toString().contains ("%"))
            {
                setterPercent(node, args..., value.toString().retainCharacters ("-1234567890.").getFloatValue());
                return true;
            }

            setter(node, args..., YGUndefined);
            return true;
        };
    }

    template <typename Setter, typename SetterPercent, typename SetterAuto, typename ...Args>
    const auto getYogaNodeDimensionAutoSetter (Setter setter, SetterPercent setterPercent, SetterAuto setterAuto, Args... args)
    {
        return [=, nonAutoSetter = getYogaNodeDimensionSetter(setter, setterPercent, args...)] (const juce::var& value, YGNodeRef node)
        {
            if (value.isString() && value.toString() == "auto")
            {
                setterAuto (node, args...);
                return true;
            }

            return nonAutoSetter (value, node);
        };
    }

    template <typename Setter, typename EnumMap>
    const auto getYogaNodeEnumSetter (Setter setter, EnumMap &map)
    {
        return [=] (const juce::var& value, YGNodeRef node)
        {
            const auto val = map.find (value);
            if (val == map.end())
            {
                // TODO catch further up to add the key at which we tried
                // to set this enum property to the message and rethrow
                throw std::invalid_argument ("Invalid property: " + value.toString().toStdString());
            }

            setter (node, val->second);
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
        /** */
        ShadowView(View* v) :
            view (v)
        {
            YGConfigSetUseWebDefaults(YGConfigGetDefault(), true);
            yogaNode = YGNodeNew();
        }

        /** */
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
            const auto it = std::find (children.begin(), children.end(), childView);

            if (it != children.end())
            {
                YGNodeRemoveChild (yogaNode, childView->yogaNode);
                children.erase (it);
            }
        }

        //==============================================================================
        /** @returns a pointer to the View instance shadowed by this node. */
        View* getAssociatedView() const noexcept { return view; }

        /** @returns the layout bounds held by the internal yogaNode. */
        juce::Rectangle<float> getCachedLayoutBounds() const
        {
            return
            {
                YGNodeLayoutGetLeft (yogaNode),
                YGNodeLayoutGetTop (yogaNode),
                YGNodeLayoutGetWidth (yogaNode),
                YGNodeLayoutGetHeight (yogaNode)
            };
        }

        /** Recursively computes the shadow tree layout. */
        void computeViewLayout (float width, float height)
        {
            YGNodeCalculateLayout (yogaNode, width, height, YGDirectionInherit);
        }

        /** Recursive traversal of the shadow tree,
            flushing layout bounds to the associated view components.
        */
        virtual void flushViewLayout()
        {
           #if JUCE_DEBUG
            if (props.contains ("debug"))
                YGNodePrint (yogaNode, (YGPrintOptions) (YGPrintOptionsLayout | YGPrintOptionsStyle | YGPrintOptionsChildren));
           #endif

            if (props.contains ("layoutAnimated"))
            {
                // Default parameters
                if (props["layoutAnimated"].isBool() && props["layoutAnimated"])
                {
                    flushViewLayoutAnimated (50.0, 45, BoundsAnimator::EasingType::linear);
                    return;
                }

                if (props["layoutAnimated"].isObject())
                {
                    const auto et = static_cast<int> (props["layoutAnimated"].getProperty ("easing", 0));

                    flushViewLayoutAnimated (static_cast<double> (props["layoutAnimated"].getProperty ("duration", 50.0)),
                                             static_cast<int> (props["layoutAnimated"].getProperty ("frameRate", 45)),
                                             static_cast<BoundsAnimator::EasingType> (et));
                    return;
                }
            }

            view->setFloatBounds (getCachedLayoutBounds());
            view->setBounds (getCachedLayoutBounds().toNearestInt());

            for (auto& child : children)
                child->flushViewLayout();
        }

        /** Recursive traversal of the shadow tree, flushing layout bounds to the
            associated view components smoothly over time. This recursive step allows
            an animation of a component subtree by just marking the parent as animated.
        */
        virtual void flushViewLayoutAnimated (double durationMs, int frameRate, BoundsAnimator::EasingType et)
        {
            animator = std::make_unique<BoundsAnimator>(
                durationMs,
                frameRate,
                et,
                view->getBounds().toFloat(),
                getCachedLayoutBounds(),
                [safeView = juce::Component::SafePointer(view)](auto && stepBounds)
                {
                    if (auto* v = safeView.getComponent())
                    {
                        v->setFloatBounds(stepBounds);
                        v->setBounds(stepBounds.toNearestInt());
                    }
                }
            );

            for (auto& child : children)
                child->flushViewLayoutAnimated(durationMs, frameRate, et);
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
