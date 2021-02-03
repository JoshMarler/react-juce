/*
  ==============================================================================

    ViewManager.cpp
    Created: 21 May 2020 15:32:00am

  ==============================================================================
*/

#include "ViewManager.h"
#include "CanvasView.h"
#include "ImageView.h"
#include "ScrollView.h"
#include "ScrollViewContentShadowView.h"
#include "TextView.h"
#include "TextInputView.h"
#include "TextShadowView.h"


namespace reactjuce
{

    namespace
    {

        /** A quick helper for registering view types. */
        template <typename ViewType, typename ShadowViewType>
        struct GenericViewFactory
        {
            ViewManager::ViewPair operator()() {
                auto view = std::make_unique<ViewType>();
                auto shadowView = std::make_unique<ShadowViewType>(view.get());

                return {std::move(view), std::move(shadowView)};
            }
        };

    }

    ViewManager::ViewManager(View* rootView)
        : rootId(rootView->getViewId())
    {
        shadowViewTable[rootId] = std::make_unique<ShadowView>(rootView);

        // Register the default view types
        registerViewType("View", GenericViewFactory<View, ShadowView>());
        registerViewType("Text", GenericViewFactory<TextView, TextShadowView>());
        registerViewType("TextInput", GenericViewFactory<TextInputView, ShadowView>());
        registerViewType("CanvasView", GenericViewFactory<CanvasView, ShadowView>());
        registerViewType("Image", GenericViewFactory<ImageView, ShadowView>());
        registerViewType("ScrollView", GenericViewFactory<ScrollView, ShadowView>());
        registerViewType("ScrollViewContentView", GenericViewFactory<View, ScrollViewContentShadowView>());
    }

    void ViewManager::registerViewType(const juce::String &typeId, ViewFactory f)
    {
        // If you hit this jassert, you're trying to register a type which
        // has already been registered!
        jassert (viewFactories.find(typeId) == viewFactories.end());
        viewFactories[typeId] = f;
    }

    ViewId ViewManager::createViewInstance(const juce::String& viewType)
    {
        // We can't create a view instance of a type that hasn't been registered.
        jassert (viewFactories.find(viewType) != viewFactories.end());

        auto [view, shadowView] = viewFactories[viewType]();
        ViewId vid = view->getViewId();

        viewTable[vid] = std::move(view);
        shadowViewTable[vid] = std::move(shadowView);

        return vid;
    }

    ViewId ViewManager::createTextViewInstance(const juce::String& value)
    {
        std::unique_ptr<View> view = std::make_unique<RawTextView>(value);
        ViewId id = view->getViewId();

        viewTable[id] = std::move(view);
        return id;
    }

    void ViewManager::setViewProperty(ViewId viewId, const juce::String& name, const juce::var& value)
    {
        const auto& [view, shadow] = getViewHandle(viewId);

        // ShadowView::setProperty returns true when a layout prop
        // has been set.  Otherwise set on the view and repaint
        if(!shadow->setProperty(name, value)) {
          view->setProperty(name, value);
          view->repaint();
        }
    }

    void ViewManager::setRawTextValue(ViewId viewId, const juce::String& value)
    {
        View* view = getViewHandle(viewId).first;

        if (auto* rawTextView = dynamic_cast<RawTextView*>(view))
        {
            // Update text
            rawTextView->setText(value);

            if (auto* parent = dynamic_cast<TextView*>(rawTextView->getParentComponent()))
            {
                // If we have a parent already, find the parent's shadow node and
                // mark it dirty, then we'll issue a new layout call
                ShadowView* parentShadowView = getViewHandle(parent->getViewId()).second;

                if (auto* textShadowView = dynamic_cast<TextShadowView*>(parentShadowView))
                {
                    textShadowView->markDirty();
                }

                // Then we need to paint, but the RawTextView has no idea how to paint its text,
                // we need to tell the parent to repaint its children.
                parent->repaint();
            }
        }
    }

    void ViewManager::insertChild(ViewId parentId, ViewId childId, int index)
    {
        // We probably don't want to be doing this!
        jassert(childId != rootId);

        const auto& [parentView, parentShadowView] = getViewHandle(parentId);
        const auto& [childView, childShadowView] = getViewHandle(childId);

        if (dynamic_cast<TextView*>(parentView))
        {
            // If we're trying to append a child to a text view, it will be raw text
            // with no accompanying shadow view, and we'll need to mark the parent
            // TextShadowView dirty before the subsequent layout pass.
            jassert (dynamic_cast<RawTextView*>(childView) != nullptr);
            jassert (childShadowView == nullptr);

            parentView->addChild(childView, index);
            dynamic_cast<TextShadowView*>(parentShadowView)->markDirty();
        }
        else
        {
            parentView->addChild(childView, index);
            parentShadowView->addChild(childShadowView, index);
        }
    }

    void ViewManager::removeChild(ViewId parentId, ViewId childId)
    {
        // We probably don't want to be doing this!
        jassert(childId != rootId);

        const auto& [parentView, parentShadowView] = getViewHandle(parentId);
        const auto& [childView, childShadowView] = getViewHandle(childId);

        // TODO: Set a View::removeChild method and call into that here. Make
        // that method virtual so that, e.g., the scroll view can override to
        // remove the child from its viewport
        parentView->removeChildComponent(childView);

        // Here we have to clear the view table of all children of this view.
        // React may clear a whole subtree from the interface by removing a
        // single component at the root of the tree. Because the view table
        // is a flat map of viewId to View, if we only remove that root view
        // from the table we leave all of its children dangling, which confuses
        // subsequent functionality like `getViewHandle` or `getViewByRefId`
        std::vector<ViewId> childIds;
        enumerateChildViewIds(childIds, childView);

        for (auto& id : childIds)
            viewTable.erase(id);

        // We might be dealing with a text view, in which case we expect a null
        // shadow view.
        if (parentShadowView && childShadowView)
        {
            parentShadowView->removeChild(childShadowView);

            // Then here, since we now know we have a child shadow view,
            // we try also to remove its children from the shadowViewTable to
            // prevent dangling children like in the viewTable above.
            for (auto& id : childIds)
                shadowViewTable.erase(id);
        }
    }

    void ViewManager::enumerateChildViewIds (std::vector<ViewId>& ids, View* v)
    {
        for (auto* child : v->getChildren())
        {
            // Some view elements may mount a plain juce::Component, such as the
            // ScrollView mounting a juce::Viewport which is a juce::Component but
            // not a juce::View. Such elements aren't in our table and can be skipped
            if (auto* childView = dynamic_cast<View*>(child))
            {
                enumerateChildViewIds(ids, childView);
            }
        }

        ids.push_back(v->getViewId());
    }

    void ViewManager::performRootShadowTreeLayout()
    {
        ShadowView* root = shadowViewTable[rootId].get();
        jassert(root);

        juce::Rectangle<float> rootBounds = root->getAssociatedView()->getLocalBounds().toFloat();
        const float width = rootBounds.getWidth();
        const float height = rootBounds.getHeight();

        root->computeViewLayout(width, height);
        root->flushViewLayout();
    }

    void ViewManager::clearViewTables()
    {
        auto nh = shadowViewTable.extract(rootId);

        viewTable.clear();
        shadowViewTable.clear();

        // Make a new root shadow view to reinitialize the view table
        shadowViewTable[rootId] = std::make_unique<ShadowView>(nh.mapped()->getAssociatedView());
    }

    juce::var ViewManager::invokeViewMethod(ViewId viewId, const juce::String &method, const juce::var::NativeFunctionArgs &args)
    {
        if (View *view = getViewHandle(viewId).first)
            return view->invokeMethod(method, args);

        throw std::logic_error("Caller attempted to invoke method on non-existent View instance");
    }

    std::pair<View*, ShadowView*> ViewManager::getViewHandle (ViewId viewId)
    {
        if (viewId == rootId)
            return {shadowViewTable[viewId]->getAssociatedView(), shadowViewTable[viewId].get()};

        if (viewTable.find(viewId) != viewTable.end())
            return {viewTable[viewId].get(), shadowViewTable[viewId].get()};

        // If we land here, you asked for a view that we don't have.
        jassertfalse;
        return {nullptr, nullptr};
    }

    View* ViewManager::getViewByRefId (const juce::Identifier& refId)
    {
        if (refId == getRootViewRefId())
            return shadowViewTable[rootId]->getAssociatedView();

        for (auto& pair : viewTable)
        {
            auto* view = pair.second.get();

            if (refId == view->getRefId())
                return view;
        }

        return nullptr;
    }

    juce::Identifier ViewManager::getRootViewRefId()
    {
       View* root = shadowViewTable[rootId]->getAssociatedView();
       jassert(root);

       return root->getRefId();
    }
}
