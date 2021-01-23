/*
  ==============================================================================

    ViewManager.h
    Created: 21 May 2020 15:32:00am

  ==============================================================================
*/

#pragma once

#include <map>

#include "View.h"
#include "ShadowView.h"


namespace reactjuce
{
    /**
     * ViewManager manages the set of Views held by a "root" View/ReactApplicationRoot instance.
     *
     * The ViewManager is responsible for adding/removing child Views to the root View's component
     * tree and registering new view types.
     *
     * ViewManager also manages triggering the layout of all View nodes within the root View's
     * tree, i.e. when a nodes is added/removed or has a property modified that requires a layout
     * change (flexbox props etc).
     * */
    class ViewManager
    {
    public:
        //==============================================================================
        // We allow registering arbitrary view types with the React context by way of
        // a "ViewFactory" here which is a user-defined function that produces a View
        // and a corresponding ShadowView.
        using ViewPair = std::pair<std::unique_ptr<View>, std::unique_ptr<ShadowView>>;
        using ViewFactory = std::function<ViewPair()>;

        //==============================================================================
        explicit ViewManager(View* rootView);
        ~ViewManager() = default;

        /** Registers a new dynamic view type and its associated factory. */
        void registerViewType(const juce::String& typeId, ViewFactory f);

        /** Creates a new view instance and registers it with the view table. */
        ViewId createViewInstance(const juce::String& viewType);

        /** Creates a new text view instance and registers it with the view table. */
        ViewId createTextViewInstance(const juce::String& value);

        /** Calls View::setProperty on the requested View */
        void setViewProperty(ViewId viewId, const juce::String& name, const juce::var& value);

        /** Calls RawTextView::setText on the requested View. If the ViewId supplied is not the id of a RawTextView
         *  this is a no-op.
         * */
        void setRawTextValue(ViewId viewId, const juce::String& value);

        /** Inserts/Appends a child View to the given parent View
         *  Callers may call without supplying index (or with index = 1)
         *  to append to the end of the parents child view list rather than
         *  inserting.
         * */
        void insertChild(ViewId parentId, ViewId childId, int index = -1);

        /** Removes a child View from the given parent View */
        void removeChild (ViewId parentId, ViewId childId);

        /** Recursively computes the shadow tree layout on the root ShadowView, then traverses the tree
            flushing new layout bounds to the associated view components.
         */
        void performRootShadowTreeLayout();

        /** Empties the internal view tables, deallocating every view except the root in the ViewManager heirarchy. */
        void clearViewTables();

        /** Invokes an exported native method on the given View instance.
         *  This method is used to allow JS code to invoke a native function on a ViewInstance using React refs.
         **/
        juce::var invokeViewMethod(ViewId viewId, const juce::String &method, const juce::var::NativeFunctionArgs &args);

        //==============================================================================
    private:
        void enumerateChildViewIds (std::vector<ViewId>& ids, View* v);

        /** Returns a pointer pair to the view associated to the given id. */
        std::pair<View*, ShadowView*> getViewHandle (ViewId viewId);

        /** Walks the view table, returning the first view with a `refId`
         *  whose value equals the provided id.
         */
        View* getViewByRefId (const juce::Identifier& refId);

        /** Helper function to return refId of the root view */
        juce::Identifier getRootViewRefId();

        ViewId                                        rootId;
        std::map<ViewId, std::unique_ptr<View>>       viewTable;
        std::map<ViewId, std::unique_ptr<ShadowView>> shadowViewTable;
        std::map<juce::String, ViewFactory>           viewFactories;
    };
}
