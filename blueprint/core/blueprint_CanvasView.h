/*
  ==============================================================================

    blueprint_CanvasView.h
    Created: 19 May 2020 9:02:25pm

  ==============================================================================
*/

#pragma once

#include "blueprint_View.h"


namespace blueprint
{

    //==============================================================================
    /** The CanvasView class is a core view for manual drawing routines within Blueprint's
        layout system.
     */
    class CanvasView : public View, public juce::Timer
    {
    public:
        //==============================================================================
        CanvasView() = default;

        ~CanvasView()
        {
            if (isTimerRunning())
                stopTimer();
        }

        //==============================================================================
        void setProperty (const juce::Identifier& name, const juce::var& value) override
        {
            View::setProperty(name, value);

            if (name == juce::Identifier("animate"))
            {
                bool shouldAnimate = value;

                if (shouldAnimate && !isTimerRunning())
                    startTimerHz(45);

                if (!shouldAnimate && isTimerRunning())
                    stopTimer();
            }
        }

        //==============================================================================
        void timerCallback() override
        {
            repaint();
        }

        //==============================================================================
        void paint (juce::Graphics& g) override
        {
            View::paint(g);

            if (props.contains("onDraw") && props["onDraw"].isMethod())
            {
                auto* jsContext = new juce::DynamicObject();
                juce::Path jsPath;

                jsContext->setProperty("__setFillStyle", juce::var::NativeFunction {
                    [&g, &jsPath](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(args.numArguments == 1);

                        auto hex = args.arguments[0].toString();
                        g.setColour(juce::Colour::fromString(hex));

                        return juce::var();
                    }
                });

                jsContext->setProperty("fillRect", juce::var::NativeFunction {
                    [&g, &jsPath](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(args.numArguments == 4);

                        const int x = args.arguments[0];
                        const int y = args.arguments[1];
                        const int width = args.arguments[2];
                        const int height = args.arguments[3];

                        g.fillRect(x, y, width, height);

                        return juce::var();
                    }
                });

                std::vector<juce::var> jsArgs {{jsContext}};
                juce::var::NativeFunctionArgs nfargs (juce::var(), jsArgs.data(), jsArgs.size());
                std::invoke(props["onDraw"].getNativeFunction(), nfargs);
            }
        }

    private:
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CanvasView)
    };

}
