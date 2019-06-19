/*
  ==============================================================================

    blueprint_View.h
    Created: 26 Nov 2018 3:38:37am

  ==============================================================================
*/

#pragma once

#include <map>


namespace blueprint
{

    // Internally we use a juce::Uuid for uniquely identifying views, but we
    // need that same identifier to make a transit through JavaScript land
    // and still match afterwards. So we map our Uuids into a signed 32-bit integer
    // type and leave Duktape to perform the appropriate cast through JavaScript's
    // double-width "Number" type.
    typedef juce::int32 ViewId;

    //==============================================================================
    /** The View class is the core component abstraction for Blueprint's declarative
        flex-based component composition.
     */
    class View : public juce::Component
    {
    public:
        //==============================================================================
        View() = default;
        virtual ~View() = default;

        //==============================================================================
        /** Returns this view's identifier. */
        ViewId getViewId();

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

        /** Dispatches a mouseDrag event to the React application. */
        void mouseDrag (const juce::MouseEvent& e) override;

    protected:
        //==============================================================================
        juce::NamedValueSet props;
        juce::Rectangle<float> cachedFloatBounds;

    private:
        //==============================================================================
        juce::Uuid _viewId;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (View)
    };

}
