#pragma once

namespace blueprint
{
    /** Internally we use a juce::Uuid for uniquely identifying views, but we
        need that same identifier to make a transit through JavaScript land
        and still match afterwards. So we map our Uuids into a signed 32-bit integer
        type and leave Duktape to perform the appropriate cast through JavaScript's
        double-width "Number" type.
    */
    using ViewId = int;

    //==============================================================================
    /** The View class is the core component abstraction for Blueprint's declarative
        flex-based component composition.
    */
    class View : public juce::Component
    {
    public:
        //==============================================================================
        /** */
        View() = default;

        //==============================================================================
        /** Returns this view's identifier. */
        ViewId getViewId() const;

        /** Returns this view's reference identifier, optionally set via React props. */
        juce::Identifier getRefId() const;

        /** Set a property on the native view. */
        virtual void setProperty (const juce::Identifier&, const juce::var&);

        /** Adds a child component behind the existing children. */
        virtual void addChild (View* childView, int index = -1);

        /** Updates the cached float layout bounds from the shadow tree. */
        void setFloatBounds (juce::Rectangle<float> bounds);

        //==============================================================================
        /** Resolves a property to a specific point value or 0 if not present. */
        float getResolvedLengthProperty (const juce::String& name, float axisLength);

        //==============================================================================
        /** Invokes, if exists, the respective view event handler. */
        void dispatchViewEvent (const juce::String& eventType, const juce::var& e);

        //==============================================================================
        /** @internal */
        void paint (juce::Graphics&) override;
        /** @internal */
        void resized() override;
        /** @internal */
        void mouseDown (const juce::MouseEvent&) override;
        /** @internal */
        void mouseUp (const juce::MouseEvent&) override;
        /** @internal */
        void mouseDrag (const juce::MouseEvent&) override;
        /** @internal */
        void mouseDoubleClick (const juce::MouseEvent&) override;
        /** @internal */
        bool keyPressed (const juce::KeyPress&) override;

    protected:
        //==============================================================================
        juce::NamedValueSet props;
        juce::Rectangle<float> cachedFloatBounds;

    private:
        //==============================================================================
        juce::Uuid _viewId;
        juce::Identifier _refId;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (View)
    };
}
