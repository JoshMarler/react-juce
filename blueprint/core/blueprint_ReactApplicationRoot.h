/*
  ==============================================================================

    blueprint_ReactApplicationRoot.h
    Created: 9 Dec 2018 10:20:37am

  ==============================================================================
*/

#pragma once

#include <map>

#include "blueprint_RawTextView.h"
#include "blueprint_ShadowView.h"
#include "blueprint_TextShadowView.h"
#include "blueprint_TextView.h"
#include "blueprint_View.h"


namespace blueprint
{

    /** This struct defines the set of functions that form the native interface
        for the JavaScript evaluation context.
     */
    struct BlueprintNative
    {
        static duk_ret_t createViewInstance (duk_context *ctx);
        static duk_ret_t createTextViewInstance (duk_context *ctx);
        static duk_ret_t setViewProperty (duk_context *ctx);
        static duk_ret_t appendChild (duk_context *ctx);
        static duk_ret_t getRootInstanceId (duk_context *ctx);
    };

    /** Allocates a new Duktape heap and initializes the BlueprintNative API therein. */
    duk_context* initializeDuktapeContext();

    //==============================================================================
    /** The ReactApplicationRoot class prepares and maintains a Duktape evaluation
        context with the relevant hooks for supporting the Blueprint render
        backend.
     */
    class ReactApplicationRoot : public View
    {
    public:
        //==============================================================================
        // TODO: This is a bad way to do this. Basically we either need to store a pointer
        // to the ReactApplicationRoot instance associated with a given Duktape context
        // either in some global static map or by storing that pointer somewhere in user
        // data inside of the Duktape context.
        static ReactApplicationRoot* singletonInstance;

        //==============================================================================
        ReactApplicationRoot()
        {
            // See note above. Currently can only create one instance.
            jassert (singletonInstance == nullptr);
            singletonInstance = this;

            // Create a duktape context
            ctx = initializeDuktapeContext();

            // Assign our root level shadow view
            _shadowView = std::make_unique<ShadowView>(this);
        }

        ~ReactApplicationRoot()
        {
            duk_destroy_heap(ctx);
        }

        //==============================================================================
        /** Override the default View behavior. */
        void resized() override
        {
            performShadowTreeLayout();
        }

        //==============================================================================
        /** Reads a JavaScript bundle from file and evaluates it in the Duktape context. */
        void runScript (const juce::File& f)
        {
            auto src = f.loadFileAsString();
            sourceFile = f;

            duk_push_string(ctx, src.toRawUTF8());

            if (duk_peval(ctx) != 0) {
                printf("Script evaluation failed: %s\n", duk_safe_to_string(ctx, -1));
            }

            duk_pop(ctx);
        }

        /** Enables keyboard focus on this component, expecting keypress events to reload
            the javascript bundle.
         */
        void enableHotkeyReloading()
        {
            setWantsKeyboardFocus(true);
        }

        /** Rebuilds a new Duktape context, reads and executes the sourceFile. */
        bool keyPressed (const juce::KeyPress& key) override
        {
            bool cmd = key.getModifiers().isCommandDown();
            auto r = key.isKeyCode(82);

            if (cmd && r)
            {
                duk_destroy_heap(ctx);
                removeAllChildren();
                viewTable.clear();
                shadowViewTable.clear();
                ctx = initializeDuktapeContext();
                _shadowView = std::make_unique<ShadowView>(this);
                runScript(sourceFile);
            }

            return true;
        }

        //==============================================================================
        // VIEW MANAGER STUFF: SPLIT OUT?

        /** Creates a new view instance and registers it with the view table. */
        ViewId createViewInstance(const juce::String& viewType)
        {
            // TODO: Next up is providing dynamic view type registration, but
            // for now we only care about View/Text.
            if (viewType == "Text")
            {
                std::unique_ptr<View> view = std::make_unique<TextView>();
                ViewId id = view->getViewId();

                viewTable[id] = std::move(view);
                shadowViewTable[id] = std::make_unique<TextShadowView>(viewTable[id].get());

                return id;
            }
            else
            {
                std::unique_ptr<View> view = std::make_unique<View>();
                ViewId id = view->getViewId();

                viewTable[id] = std::move(view);
                shadowViewTable[id] = std::make_unique<ShadowView>(viewTable[id].get());

                return id;
            }
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

        void appendChild (ViewId parentId, ViewId childId)
        {
            const auto& [parentView, parentShadowView] = getViewHandle(parentId);
            const auto& [childView, childShadowView] = getViewHandle(childId);

            parentView->appendChild(childView);
            parentShadowView->appendChild(childShadowView);

            performShadowTreeLayout();
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

    private:
        //==============================================================================
        std::unique_ptr<ShadowView> _shadowView;
        std::map<ViewId, std::unique_ptr<View>> viewTable;
        std::map<ViewId, std::unique_ptr<ShadowView>> shadowViewTable;

        juce::File sourceFile;
        duk_context* ctx;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReactApplicationRoot)
    };

}
