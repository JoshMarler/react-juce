#pragma once

#include "View.h"


namespace reactjuce
{

    //==============================================================================
    struct BoundsAnimator : public juce::Timer
    {
        using StepCallback = std::function<void(juce::Rectangle<float>)>;

        enum class EasingType
        {
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

        BoundsAnimator(double durationMs,
                       int frameRateToUse,
                       EasingType et,
                       juce::Rectangle<float> startRect,
                       juce::Rectangle<float> destRect,
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

        ~BoundsAnimator() override
        {
            stopTimer();
        }

        static constexpr float  lerp (float a, float b, double t)  { return a + (static_cast<float> (t) * (b - a)); }

        void timerCallback() override
        {
            auto now = juce::Time::getMillisecondCounterHiRes();
            double t = std::clamp((now - startTime) / duration, 0.0, 1.0);

            // Super helpful cheat sheet: https://gist.github.com/gre/1650294
            switch (easingType)
            {
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

            if (t >= 0.9999)
            {
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
        explicit ShadowView(View* _view);
        virtual ~ShadowView();

        //==============================================================================
        /** Set a property on the shadow view. */
        virtual bool setProperty (const juce::String& name, const juce::var& newValue);

        /** Adds a child component behind the existing children. */
        //TODO: Deal with default arg. Virtual functions shouldn't have them.
        virtual void addChild (ShadowView* childView, int index = -1);

        /** Removes a child component from the children array. */
        virtual void removeChild (ShadowView* childView);

        //==============================================================================
        /** Returns a pointer to the View instance shadowed by this node. */
        View* getAssociatedView();

        //==============================================================================
        /** Returns the layout bounds held by the internal yogaNode. */
        juce::Rectangle<float> getCachedLayoutBounds();

        /** Recursively computes the shadow tree layout. */
        void computeViewLayout(float width, float height);

        /** Recursive traversal of the shadow tree, flushing layout bounds to
            the associated view components.
         */
        virtual void flushViewLayout();

        /** Recursive traversal of the shadow tree, flushing layout bounds to the
         *  associated view components smoothly over time. This recursive step allows
         *  an animation of a component subtree by just marking the parent as animated.
         */
        virtual void flushViewLayoutAnimated(double durationMs, int frameRate, BoundsAnimator::EasingType et);

    protected:
        //==============================================================================
        std::vector<ShadowView*>& getChildren();

        class ShadowViewPimpl;
        ShadowViewPimpl& getShadowViewImpl();

    private:
        //==============================================================================
        std::unique_ptr<ShadowViewPimpl> shadowViewPimpl;
        friend ShadowViewPimpl;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShadowView)
    };

}
