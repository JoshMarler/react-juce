/*
  ==============================================================================

    blueprint_ShadowView.h
    Created: 17 Apr 2019 8:38:37am

  ==============================================================================
*/

#pragma once

#include "blueprint_View.h"


namespace blueprint
{

    //==============================================================================
    /** The ShadowView class decouples layout constraints from the actual View instances
        so that our View tree and ShadowView tree might differ (i.e. in the case of raw
        text nodes), and so that our View may remain more simple.
     */
    class ShadowView
    {
    public:
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
        virtual void setProperty (const juce::Identifier& name, const juce::var& newValue);

        /** Adds a child component behind the existing children. */
        virtual void appendChild (ShadowView* childView)
        {
            YGNodeInsertChild(yogaNode, childView->yogaNode, YGNodeGetChildCount(yogaNode));
            children.push_back(childView);
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
        void flushViewLayout()
        {
            view->setBounds(getCachedLayoutBounds().toNearestInt());

            if (props.contains("debug"))
                YGNodePrint(yogaNode, (YGPrintOptions) (YGPrintOptionsLayout
                                                        | YGPrintOptionsStyle
                                                        | YGPrintOptionsChildren));

            for (auto& child : children)
                child->flushViewLayout();
        }

    protected:
        //==============================================================================
        YGNodeRef yogaNode;

    private:
        //==============================================================================
        View* view = nullptr;
        juce::NamedValueSet props;

        std::vector<ShadowView*> children;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShadowView)
    };

}
