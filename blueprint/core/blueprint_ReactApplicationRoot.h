/*
  ==============================================================================

    blueprint_ReactApplicationRoot.h
    Created: 9 Dec 2018 10:20:37am

  ==============================================================================
*/

#pragma once

#include <map>

#include "blueprint_EcmascriptEngine.h"
#include "blueprint_ImageView.h"
#include "blueprint_RawTextView.h"
#include "blueprint_ScrollView.h"
#include "blueprint_ScrollViewContentShadowView.h"
#include "blueprint_ShadowView.h"
#include "blueprint_TextShadowView.h"
#include "blueprint_TextView.h"
#include "blueprint_View.h"


namespace blueprint
{

    //==============================================================================
    /** The ReactApplicationRoot class prepares and maintains a Duktape evaluation
        context with the relevant hooks for supporting the Blueprint render
        backend.
     */
    class ReactApplicationRoot : public View
    {
    public:
        //==============================================================================
        // We allow registering arbitrary view types with the React context by way of
        // a "ViewFactory" here which is a user-defined function that produces a View
        // and a corresponding ShadowView.
        typedef std::pair<std::unique_ptr<View>, std::unique_ptr<ShadowView>> ViewPair;
        typedef std::function<ViewPair()> ViewFactory;

        //==============================================================================
        ReactApplicationRoot()
        {
            JUCE_ASSERT_MESSAGE_THREAD

            // Install React.js backend rendering methods
            registerNativeRenderingHooks();

            // Assign our root level shadow view
            _shadowView = std::make_unique<ShadowView>(this);

            // And install view types
            installNativeViewTypes();
        }

        //==============================================================================
        /** Override the default View behavior. */
        void resized() override
        {
            performShadowTreeLayout();
        }

        //==============================================================================
        // VIEW MANAGER STUFF: SPLIT OUT?

        /** Registers a new dynamic view type and its associated factory. */
        void registerViewType(const juce::String& typeId, ViewFactory f)
        {
            // If you hit this jassert, you're trying to register a type which
            // has already been registered!
            jassert (viewFactories.find(typeId) == viewFactories.end());
            viewFactories[typeId] = f;
        }

        /** Creates a new view instance and registers it with the view table. */
        ViewId createViewInstance(const juce::String& viewType)
        {
            // We can't create a view instance of a type that hasn't been registered.
            jassert (viewFactories.find(viewType) != viewFactories.end());

            auto [view, shadowView] = viewFactories[viewType]();
            ViewId vid = view->getViewId();

            viewTable[vid] = std::move(view);
            shadowViewTable[vid] = std::move(shadowView);

            return vid;
        }

        /** Creates a new text view instance and registers it with the view table. */
        ViewId createTextViewInstance(const juce::String& value)
        {
            std::unique_ptr<View> view = std::make_unique<RawTextView>(value);
            ViewId id = view->getViewId();

            viewTable[id] = std::move(view);
            return id;
        }

        void setViewProperty (ViewId viewId, const juce::Identifier& name, const juce::var& value)
        {
            const auto& [view, shadow] = getViewHandle(viewId);

            view->setProperty(name, value);
            shadow->setProperty(name, value);

            // For now, we just assume that any new property update means we
            // need to redraw or lay out our tree again. This is an easy future
            // optimization.
            performShadowTreeLayout();
            view->repaint();
        }

        void setRawTextValue (ViewId viewId, const juce::String& value)
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
                        performShadowTreeLayout();
                    }

                    // Then we need to paint, but the RawTextView has no idea how to paint its text,
                    // we need to tell the parent to repaint its children.
                    parent->repaint();
                }
            }
        }

        void addChild (ViewId parentId, ViewId childId, int index = -1)
        {
            const auto& [parentView, parentShadowView] = getViewHandle(parentId);
            const auto& [childView, childShadowView] = getViewHandle(childId);

            if (auto* textView = dynamic_cast<TextView*>(parentView))
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

            performShadowTreeLayout();
        }

        void removeChild (ViewId parentId, ViewId childId)
        {
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

            performShadowTreeLayout();
        }

        void enumerateChildViewIds (std::vector<ViewId>& ids, View* v)
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

        /** Returns a pointer pair to the view associated to the given id. */
        std::pair<View*, ShadowView*> getViewHandle (ViewId viewId)
        {
            if (viewId == getViewId())
                return {this, _shadowView.get()};

            if (viewTable.find(viewId) != viewTable.end())
                return {viewTable[viewId].get(), shadowViewTable[viewId].get()};

            // If we land here, you asked for a view that we don't have.
            jassertfalse;
            return {nullptr, nullptr};
        }

        /** Walks the view table, returning the first view with a `refId`
         *  whose value equals the provided id.
         */
        View* getViewByRefId (const juce::Identifier& refId)
        {
            if (refId == getRefId())
                return this;

            for (auto& pair : viewTable)
            {
                auto* view = pair.second.get();

                if (refId == view->getRefId())
                    return view;
            }

            return nullptr;
        }

        /** Recursively computes the shadow tree layout, then traverses the tree
            flushing new layout bounds to the associated view components.
         */
        void performShadowTreeLayout()
        {
            juce::Rectangle<float> bounds = getLocalBounds().toFloat();
            const float width = bounds.getWidth();
            const float height = bounds.getHeight();

            _shadowView->computeViewLayout(width, height);
            _shadowView->flushViewLayout();
        }

        //==============================================================================
        /** Evaluates a javascript bundle in the Ecmascript engine. */
        juce::var evaluate (const juce::String& bundle)
        {
            JUCE_ASSERT_MESSAGE_THREAD
            return engine.evaluate(bundle);
        }

        /** Dispatches an event to the React internal view registry.

            If the view given by the `viewId` has a handler for the given event, it
            will be called with the given arguments.
         */
        template <typename... T>
        void dispatchViewEvent (ViewId viewId, const juce::String& eventType, T... args)
        {
            JUCE_ASSERT_MESSAGE_THREAD
            engine.invoke("__BlueprintNative__.dispatchViewEvent", viewId, eventType, std::forward<T>(args)...);
        }

        /** Dispatches an event through Blueprint's EventBridge. */
        template <typename... T>
        void dispatchEvent (const juce::String& eventType, T... args)
        {
            JUCE_ASSERT_MESSAGE_THREAD
            engine.invoke("__BlueprintNative__.dispatchEvent", eventType, std::forward<T>(args)...);
        }

        //==============================================================================
        EcmascriptEngine engine;

    private:
        //==============================================================================
        /** Registers each of the natively supported view types. */
        void installNativeViewTypes()
        {
            registerViewType("Text", []() -> ViewPair {
                auto view = std::make_unique<TextView>();
                auto shadowView = std::make_unique<TextShadowView>(view.get());

                return {std::move(view), std::move(shadowView)};
            });

            registerViewType("View", []() -> ViewPair {
                auto view = std::make_unique<View>();
                auto shadowView = std::make_unique<ShadowView>(view.get());

                return {std::move(view), std::move(shadowView)};
            });

            registerViewType("Image", []() -> ViewPair {
                auto view = std::make_unique<ImageView>();

                // ImageView does not need a specialized shadow view, unless
                // we want to enforce at the ShadowView level that it cannot
                // take children.
                auto shadowView = std::make_unique<ShadowView>(view.get());

                return {std::move(view), std::move(shadowView)};
            });

            registerViewType("ScrollView", []() -> ViewPair {
                auto view = std::make_unique<ScrollView>();
                auto shadowView = std::make_unique<ShadowView>(view.get());

                return {std::move(view), std::move(shadowView)};
            });

            registerViewType("ScrollViewContentView", []() -> ViewPair {
                auto view = std::make_unique<View>();
                auto shadowView = std::make_unique<ScrollViewContentShadowView>(view.get());

                return {std::move(view), std::move(shadowView)};
            });
        }

        void registerNativeRenderingHooks()
        {
            engine.registerNativeProperty("__BlueprintNative__", juce::JSON::parse("{}"));

            engine.registerNativeMethod("__BlueprintNative__", "createViewInstance", [](void* stash, const juce::var::NativeFunctionArgs& args) {
                ReactApplicationRoot* self = reinterpret_cast<ReactApplicationRoot*>(stash);

                jassert (self != nullptr);
                jassert (args.numArguments == 1);

                juce::String viewType = args.arguments[0].toString();
                ViewId viewId = self->createViewInstance(viewType);

                return juce::var(viewId);
            }, (void *) this);

            engine.registerNativeMethod("__BlueprintNative__", "createTextViewInstance", [](void* stash, const juce::var::NativeFunctionArgs& args) {
                ReactApplicationRoot* self = reinterpret_cast<ReactApplicationRoot*>(stash);

                jassert (self != nullptr);
                jassert (args.numArguments == 1);

                juce::String textValue = args.arguments[0].toString();
                ViewId viewId = self->createTextViewInstance(textValue);

                return juce::var(viewId);
            }, (void *) this);

            engine.registerNativeMethod("__BlueprintNative__", "setViewProperty", [](void* stash, const juce::var::NativeFunctionArgs& args) {
                ReactApplicationRoot* self = reinterpret_cast<ReactApplicationRoot*>(stash);

                jassert (self != nullptr);
                jassert (args.numArguments == 3);

                ViewId viewId = args.arguments[0];
                auto propertyName = args.arguments[1].toString();
                auto propertyValue = args.arguments[2];

                self->setViewProperty(viewId, propertyName, propertyValue);
                return juce::var::undefined();
            }, (void *) this);

            engine.registerNativeMethod("__BlueprintNative__", "setRawTextValue", [](void* stash, const juce::var::NativeFunctionArgs& args) {
                ReactApplicationRoot* self = reinterpret_cast<ReactApplicationRoot*>(stash);

                jassert (self != nullptr);
                jassert (args.numArguments == 2);

                ViewId viewId = args.arguments[0];
                auto textValue = args.arguments[1].toString();

                self->setRawTextValue(viewId, textValue);
                return juce::var::undefined();
            }, (void *) this);

            engine.registerNativeMethod("__BlueprintNative__", "addChild", [](void* stash, const juce::var::NativeFunctionArgs& args) {
                ReactApplicationRoot* self = reinterpret_cast<ReactApplicationRoot*>(stash);

                jassert (self != nullptr);
                jassert (args.numArguments >= 2);

                ViewId parentId = args.arguments[0];
                ViewId childId = args.arguments[1];
                int index = -1;

                if (args.numArguments > 2)
                    index = args.arguments[2];

                self->addChild(parentId, childId, index);
                return juce::var::undefined();
            }, (void *) this);

            engine.registerNativeMethod("__BlueprintNative__", "removeChild", [](void* stash, const juce::var::NativeFunctionArgs& args) {
                ReactApplicationRoot* self = reinterpret_cast<ReactApplicationRoot*>(stash);

                jassert (self != nullptr);
                jassert (args.numArguments == 2);

                ViewId parentId = args.arguments[0];
                ViewId childId = args.arguments[1];

                self->removeChild(parentId, childId);
                return juce::var::undefined();
            }, (void *) this);

            engine.registerNativeMethod("__BlueprintNative__", "getRootInstanceId", [](void* stash, const juce::var::NativeFunctionArgs& args) {
                ReactApplicationRoot* self = reinterpret_cast<ReactApplicationRoot*>(stash);

                jassert (self != nullptr);
                jassert (args.numArguments == 0);

                return juce::var(self->getViewId());
            }, (void *) this);
        }

        //==============================================================================
        std::unique_ptr<ShadowView> _shadowView;
        std::map<ViewId, std::unique_ptr<View>> viewTable;
        std::map<ViewId, std::unique_ptr<ShadowView>> shadowViewTable;
        std::map<juce::String, ViewFactory> viewFactories;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReactApplicationRoot)
    };

}
