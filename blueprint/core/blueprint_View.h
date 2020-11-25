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
        /** Represents a Touch object. */
        struct Touch
        {
            Touch (const int identifier, const int x, const int y, const int screenX, const int screenY, const juce::var target)
                : identifier(identifier), x(x), y(y), screenX(x), screenY(screenY), target(target)
            {}

            void setCoordinatesFromEvent(const juce::MouseEvent& e)
            {
                x = e.x;
                y = e.y;
                screenX = e.getScreenX();
                screenY = e.getScreenY();
            }

            juce::var getVar()
            {
                juce::String jsonString = "{\"identifier\": " + juce::String(identifier) + ",\"x\": " + juce::String(x)+ ",\"y\": " + juce::String(y)+ ",\"screenX\": " + juce::String(screenX)+ ",\"screenY\": " + juce::String(screenY)+ ",\"target\": " + target.toString() + "}";
                return juce::JSON::parse(jsonString);
            }

            int identifier;
            int x;
            int y;
            int screenX;
            int screenY;
            juce::var target;

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Touch)
        };

        /** Lists used to represent the properties of TouchEvents. */
        juce::Array<Touch*> touches;
        juce::Array<Touch*> targetTouches;
        juce::Array<Touch*> changedTouches;

        /** Retrieve a specific Touch object in an array of Touch objects, if it exists. */
        static View::Touch* getTouch (const int touchIdentifier, juce::Array<Touch*> myList)
        {
            for (int i = 0; i < myList.size(); ++i)
            {
                Touch* t = myList.getUnchecked(i);

                if (t->identifier == touchIdentifier)
                    return t;
            }

            return nullptr;
        }

        /** Retrieve the index of a specific Touch object in an array of Touch objects, if it exists. */
        static int getTouchIndexInList (const int touchIdentifier, juce::Array<Touch*> myList)
        {
            for (int i = 0; i < myList.size(); ++i)
            {
                Touch* t = myList.getUnchecked(i);

                if (t->identifier == touchIdentifier)
                    return i;
            }

            return -1;
        }


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
