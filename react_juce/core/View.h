/*
  ==============================================================================

    View.h
    Created: 26 Nov 2018 3:38:37am

  ==============================================================================
*/

#pragma once

#include <map>


namespace reactjuce
{

    // Internally we use a juce::Uuid for uniquely identifying views, but we
    // need that same identifier to make a transit through JavaScript land
    // and still match afterwards. So we map our Uuids into a signed 32-bit integer
    // type and leave Duktape to perform the appropriate cast through JavaScript's
    // double-width "Number" type.
    using ViewId = juce::int32;

    //==============================================================================
    /** The View class is the core component abstraction for React-JUCE's declarative
        flex-based component composition.
     */
    class View : public juce::Component
    {
    public:
        //==============================================================================
        static const inline juce::Identifier interceptClickEventsProp = "interceptClickEvents";
        static const inline juce::Identifier onKeyPressProp           = "onKeyPress";
        static const inline juce::Identifier opacityProp              = "opacity";
        static const inline juce::Identifier refIdProp                = "refId";
        static const inline juce::Identifier transformMatrixProp      = "transform-matrix";

        static const inline juce::Identifier backgroundColorProp      = "background-color";

        static const inline juce::Identifier borderColorProp          = "border-color";
        static const inline juce::Identifier borderPathProp           = "border-path";
        static const inline juce::Identifier borderRadiusProp         = "border-radius";
        static const inline juce::Identifier borderWidthProp          = "border-width";

        //==============================================================================
        View() = default;
        ~View() override = default;

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

        /** Override the default Component method with default paint behaviors. */
        void paint (juce::Graphics& g) override;

        //==============================================================================
        /** Dispatches a resized event to the React application. */
        void resized() override;

        /** Dispatches a mouseDown event to the React application. */
        void mouseDown (const juce::MouseEvent& e) override;

        /** Dispatches a mouseUp event to the React application. */
        void mouseUp (const juce::MouseEvent& e) override;

        /** Dispatches a mouseEnter event to the React application. */
        void mouseEnter (const juce::MouseEvent& e) override;

        /** Dispatches a mouseLeave event to the React application. */
        void mouseExit (const juce::MouseEvent& e) override;

        /** Dispatches a mouseDrag event to the React application. */
        void mouseDrag (const juce::MouseEvent& e) override;

        /** Dispatches a mouseDoubleClick event to the React application. */
        void mouseDoubleClick (const juce::MouseEvent& e) override;

        /** Dispatches a keyPress event to the React application. */
        bool keyPressed (const juce::KeyPress& e) override;

        //==============================================================================
        /** Invokes, if exists, the respective view event handler. */
        void dispatchViewEvent (const juce::String& eventType, const juce::var& e);

        //==============================================================================
        /** Invokes an "exported" native method on the View instance */
        juce::var invokeMethod(const juce::String &method, const juce::var::NativeFunctionArgs &args);

    protected:
        //==============================================================================
        /** Exports/Registers a method on this View instance so it may be called
         *  directly from React. This is here to support calling ViewInstance functions
         *  in React via component refs.
         * */
        void exportMethod(const juce::String &method, juce::var::NativeFunction fn);

        //==============================================================================
        juce::NamedValueSet props;
        juce::Rectangle<float> cachedFloatBounds;

    private:
        //==============================================================================
        juce::Uuid _viewId;
        juce::Identifier _refId;

        std::unordered_map<juce::String, juce::var::NativeFunction> nativeMethods;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (View)
    };

}
