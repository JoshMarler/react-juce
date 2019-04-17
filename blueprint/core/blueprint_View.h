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
        void appendChild (View* childView);

        //==============================================================================
        /** Override the default Component method with default paint behaviors. */
        void paint (juce::Graphics& g) override;

    private:
        //==============================================================================
        juce::Uuid _viewId;
        juce::NamedValueSet props;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (View)
    };

}
