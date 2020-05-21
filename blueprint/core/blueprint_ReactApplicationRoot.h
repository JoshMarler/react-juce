/*
  ==============================================================================

    blueprint_ReactApplicationRoot.h
    Created: 9 Dec 2018 10:20:37am

  ==============================================================================
*/

#pragma once

#include <map>

#include "blueprint_CanvasView.h"
#include "blueprint_EcmascriptEngine.h"
#include "blueprint_ImageView.h"
#include "blueprint_RawTextView.h"
#include "blueprint_ScrollView.h"
#include "blueprint_ScrollViewContentShadowView.h"
#include "blueprint_ShadowView.h"
#include "blueprint_TextShadowView.h"
#include "blueprint_TextView.h"
#include "blueprint_View.h"
#include "blueprint_ViewManager.h"

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
        ReactApplicationRoot()
        {
            JUCE_ASSERT_MESSAGE_THREAD

            _viewManager = std::make_unique<ViewManager>(this);

            // Install React.js backend rendering methods
            registerNativeRenderingHooks();

            // And install view types
            installNativeViewTypes();
        }

        //==============================================================================
        /** Override the default View behavior. Currently a no-op but we do not want to call the base View::resized here */
        void resized() override
        {
            jassert(_viewManager);
            _viewManager->performRootShadowTreeLayout();
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
        void registerViewType(const juce::String& typeId, ViewManager::ViewFactory f)
        {
            JUCE_ASSERT_MESSAGE_THREAD

            jassert(_viewManager);
            _viewManager->registerViewType(typeId, f);
        }

        ViewManager* const getViewManager()
        {
            JUCE_ASSERT_MESSAGE_THREAD
            return _viewManager.get();
        }

        //==============================================================================
        EcmascriptEngine engine;

    private:
        //==============================================================================
        /** Registers each of the natively supported view types. */
        void installNativeViewTypes()
        {
            using ViewPair = ViewManager::ViewPair;

            jassert(_viewManager);

            _viewManager->registerViewType("Text", []() -> ViewPair {
                auto view = std::make_unique<TextView>();
                auto shadowView = std::make_unique<TextShadowView>(view.get());

                return {std::move(view), std::move(shadowView)};
            });

            _viewManager->registerViewType("View", []() -> ViewPair {
                auto view = std::make_unique<View>();
                auto shadowView = std::make_unique<ShadowView>(view.get());

                return {std::move(view), std::move(shadowView)};
            });

            _viewManager->registerViewType("CanvasView", []() -> ViewPair {
                auto view = std::make_unique<CanvasView>();
                auto shadowView = std::make_unique<ShadowView>(view.get());

                return {std::move(view), std::move(shadowView)};
            });

            _viewManager->registerViewType("Image", []() -> ViewPair {
                auto view = std::make_unique<ImageView>();

                // ImageView does not need a specialized shadow view, unless
                // we want to enforce at the ShadowView level that it cannot
                // take children.
                auto shadowView = std::make_unique<ShadowView>(view.get());

                return {std::move(view), std::move(shadowView)};
            });

            _viewManager->registerViewType("ScrollView", []() -> ViewPair {
                auto view = std::make_unique<ScrollView>();
                auto shadowView = std::make_unique<ShadowView>(view.get());

                return {std::move(view), std::move(shadowView)};
            });

            _viewManager->registerViewType("ScrollViewContentView", []() -> ViewPair {
                auto view = std::make_unique<View>();
                auto shadowView = std::make_unique<ScrollViewContentShadowView>(view.get());

                return {std::move(view), std::move(shadowView)};
            });
        }

        void registerNativeRenderingHooks()
        {
            engine.registerNativeProperty("__BlueprintNative__", juce::JSON::parse("{}"));

            engine.registerNativeMethod("__BlueprintNative__", "createViewInstance", [](void* stash, const juce::var::NativeFunctionArgs& args) {
                auto self = reinterpret_cast<ReactApplicationRoot*>(stash);

                jassert (self != nullptr);
                jassert (self->_viewManager);
                jassert (args.numArguments == 1);

                ViewManager* const viewManager = self->getViewManager();
                jassert(viewManager);

                juce::String viewType = args.arguments[0].toString();
                ViewId viewId = viewManager->createViewInstance(viewType);

                return juce::var(viewId);
            }, (void *) this);

            engine.registerNativeMethod("__BlueprintNative__", "createTextViewInstance", [](void* stash, const juce::var::NativeFunctionArgs& args) {
                auto self = reinterpret_cast<ReactApplicationRoot*>(stash);

                jassert (self != nullptr);
                jassert (args.numArguments == 1);

                ViewManager* const viewManager = self->getViewManager();
                jassert(viewManager);

                juce::String textValue = args.arguments[0].toString();
                ViewId viewId = viewManager->createTextViewInstance(textValue);

                return juce::var(viewId);
            }, (void *) this);

            engine.registerNativeMethod("__BlueprintNative__", "setViewProperty", [](void* stash, const juce::var::NativeFunctionArgs& args) {
                auto self = reinterpret_cast<ReactApplicationRoot*>(stash);

                jassert (self != nullptr);
                jassert (args.numArguments == 3);

                ViewId viewId = args.arguments[0];
                auto propertyName = args.arguments[1].toString();
                auto propertyValue = args.arguments[2];

                ViewManager* const viewManager = self->getViewManager();
                jassert(viewManager);

                viewManager->setViewProperty(viewId, propertyName, propertyValue);
                return juce::var::undefined();
            }, (void *) this);

            engine.registerNativeMethod("__BlueprintNative__", "setRawTextValue", [](void* stash, const juce::var::NativeFunctionArgs& args) {
                auto self = reinterpret_cast<ReactApplicationRoot*>(stash);

                jassert (self != nullptr);
                jassert (args.numArguments == 2);

                ViewId viewId = args.arguments[0];
                auto textValue = args.arguments[1].toString();

                ViewManager* const viewManager = self->getViewManager();
                jassert(viewManager);

                viewManager->setRawTextValue(viewId, textValue);
                return juce::var::undefined();
            }, (void *) this);

            engine.registerNativeMethod("__BlueprintNative__", "addChild", [](void* stash, const juce::var::NativeFunctionArgs& args) {
                auto self = reinterpret_cast<ReactApplicationRoot*>(stash);

                jassert (self != nullptr);
                jassert (args.numArguments >= 2);

                ViewId parentId = args.arguments[0];
                ViewId childId = args.arguments[1];
                int index = -1;

                if (args.numArguments > 2)
                    index = args.arguments[2];

                ViewManager* const viewManager = self->getViewManager();
                jassert(viewManager);

                viewManager->addChild(parentId, childId, index);
                return juce::var::undefined();
            }, (void *) this);

            engine.registerNativeMethod("__BlueprintNative__", "removeChild", [](void* stash, const juce::var::NativeFunctionArgs& args) {
                auto self = reinterpret_cast<ReactApplicationRoot*>(stash);

                jassert (self != nullptr);
                jassert (args.numArguments == 2);

                ViewId parentId = args.arguments[0];
                ViewId childId = args.arguments[1];

                ViewManager* const viewManager = self->getViewManager();
                jassert(viewManager);

                viewManager->removeChild(parentId, childId);
                return juce::var::undefined();
            }, (void *) this);

            engine.registerNativeMethod("__BlueprintNative__", "getRootInstanceId", [](void* stash, const juce::var::NativeFunctionArgs& args) {
                auto self = reinterpret_cast<ReactApplicationRoot*>(stash);

                jassert (self != nullptr);
                jassert (args.numArguments == 0);

                return juce::var(self->getViewId());
            }, (void *) this);
        }

        //==============================================================================
        std::unique_ptr<ViewManager> _viewManager;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReactApplicationRoot)
    };
}
