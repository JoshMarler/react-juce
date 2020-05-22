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
    /** The CanvasContext class is a C++ implementation/binding for the
     *  HTML Canvas API's CanvasRenderingContext2D object.
     *
     *  https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D
     *
     *  CanvasContext is used by the CanvasView class to support HTML <canvas> like rendering in React/Blueprint
     *  components. CanvasContext could also be used outside of the CanvasView element in things like juce::LookAndFeel
     *  overrides.
     *
     *  CanvasContext draws to a juce::Image internally.
     *
     *  To use CanvasContext you simply need to instantiate a CanvasContext instance, call init() and then pass the
     *  CanvasContext instance to your JS drawing routine. Your JS drawing routine will then render to the
     *  CanvasContext's juce::Image when invoked, this juce::Image instance can then be rendered anywhere using
     *  CanvasContext::getImage() and juce::Graphics::drawImageAt(). See the CanvasView::paint() implementation
     *  for an example of this.
     *
     *  Note that is is important to call CanvasContext::setSize() before invoking your JS drawing routine.
     *
     *  A typical JS drawing routine that uses this implementation of the CanvasRenderingContext2D may look like the
     *  below:
     *
     * @code
     *      function drawCircle(ctx, x, y, radius, filled) {
     *          ctx.beginPath();
     *          ctx.moveTo(x + radius, y);
     *          ctx.arc(x, y, radius, 0, 2 * Math.PI);
     *
     *          if (filled)
     *              ctx.fill();
     *          else
     *              ctx.stroke();
     *      }
     *
     *      function doDraw(ctx) {
     *          ctx.strokeColour = `ff884848`;
     *          ctx.fillColour   = `ffffffaa`;
     *
     *          drawCircle(ctx, x, y, radius, true);
     *      }
     * @endcode
     *
     */
    class CanvasContext : public juce::DynamicObject
    {
    public:
        using FillStyle   = juce::FillType;
        using StrokeStyle = juce::FillType;

        struct Properties
        {
            FillStyle      fillStyle{};
            StrokeStyle    strokeStyle{};
            int            lineWidth = 1;
        };

        explicit CanvasContext()
            : image(juce::Image::ARGB, 1, 1, true)
            , graphics(std::make_unique<juce::Graphics>(image))
        {
            registerNativeProperties();
            registerNativeFunctions();
        }

        ~CanvasContext() = default;

        /** Set the size of the CanvasContext. This is akin to setting the width and height attributes
         *  on a HTML <canvas> element. If you have a blueprint::View or a juce::Component that wished
         *  to render a CanvasContext you should ensure CanvasContext::setSize() is called from the
         *  View/Component's resized function.
         */
        void setSize(int width, int height)
        {
            image = juce::Image(juce::Image::ARGB, width, height, true);
        }

        /** Get the juce::Image instance that has been rendered to. This can be drawn to the screen
         *  using juce::Graphics::drawImageAt().
         */
        const juce::Image& getImage() const
        {
            return image;
        }

        /**  init should be called before passing the CanvasContext instance to a JS drawing function. */
        void init()
        {
            graphics = std::make_unique<juce::Graphics>(image);
        }

    private:
        void registerNativeProperties()
        {
            setProperty("__setFillStyle", juce::var::NativeFunction {
                    [=](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(graphics);
                        jassert(args.numArguments == 1);

                        //TODO: Implement fillStyle pattern
                        //TODO: Implement fillStyle gradient

                        const auto colourHex = args.arguments[0].toString();
                        properties.fillStyle.setColour(juce::Colour::fromString(colourHex));

                        graphics->setFillType(properties.fillStyle);

                        return juce::var();
                    }
            });

            setProperty("__setStrokeStyle", juce::var::NativeFunction {
                    [=](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(graphics);
                        jassert(args.numArguments == 1);

                        //TODO: Implement strokeStyle pattern
                        //TODO: Implement stokeStyle gradient

                        const auto colourHex = args.arguments[0].toString();
                        properties.strokeStyle.setColour(juce::Colour::fromString(colourHex));

                        return juce::var();
                    }
            });

            setProperty("__setLineWidth", juce::var::NativeFunction {
                    [=](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(graphics);
                        jassert(args.numArguments == 1);

                        const int lineWidth = args.arguments[0];
                        properties.lineWidth = lineWidth;

                        return juce::var();
                    }
            });
        }

        void registerNativeFunctions()
        {
//            setProperty("rect", juce::var::NativeFunction {
//                    [=](const juce::var::NativeFunctionArgs& args) -> juce::var {
//                        jassert(args.numArguments == 4);
//
//                        const float x = args.arguments[0];
//                        const float y = args.arguments[1];
//                        const float width = args.arguments[2];
//                        const float height = args.arguments[3];
//
//                        // TODO: For some reason this operation closes the current path.
//                        //       The API docs suggest addRectangle should be added as a new sub-path
//                        //       leaving the previous path open.
//                        //       Fix.
//                        path.addRectangle(x, y, width, height);
//
//                        return juce::var();
//                    }
//            });

            setProperty("fillRect", juce::var::NativeFunction {
                    [=](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(graphics);
                        jassert(args.numArguments == 4);

                        const int x = args.arguments[0];
                        const int y = args.arguments[1];
                        const int width = args.arguments[2];
                        const int height = args.arguments[3];

                        graphics->fillRect(x, y, width, height);

                        return juce::var();
                    }
            });

            setProperty("strokeRect", juce::var::NativeFunction {
                    [=](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(graphics);
                        jassert(args.numArguments == 4);

                        const int x = args.arguments[0];
                        const int y = args.arguments[1];
                        const int width = args.arguments[2];
                        const int height = args.arguments[3];

                        graphics->drawRect(x, y, width, height, properties.lineWidth);

                        return juce::var();
                    }
            });

            //TODO: Add clearRect. Use View background color and g.fillRect?

            // Path functions
            setProperty("beginPath", juce::var::NativeFunction {
                    [=](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(graphics);
                        jassert(args.numArguments == 0);

                        // Reset the current path on a call to beginPath on the context.
                        path = juce::Path();

                        return juce::var();
                    }
            });

            setProperty("lineTo", juce::var::NativeFunction {
                    [=](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(graphics);
                        jassert(args.numArguments == 2);

                        const float x = args.arguments[0];
                        const float y = args.arguments[1];

                        path.lineTo(x, y);

                        return juce::var();
                    }
            });

            setProperty("moveTo", juce::var::NativeFunction {
                    [=](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(graphics);
                        jassert(args.numArguments == 2);

                        const float x = args.arguments[0];
                        const float y = args.arguments[1];

                        path.startNewSubPath(x, y);

                        return juce::var();
                    }
            });

            setProperty("arc", juce::var::NativeFunction {
                    [=](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(graphics);

                        // CanvasRenderingContext2D.arc() uses default antiClockWise false arg
                        jassert(args.numArguments >= 5 && args.numArguments <= 6);

                        const float x             = args.arguments[0];
                        const float y             = args.arguments[1];
                        const float radius        = args.arguments[2];
                        const float startAngle    = args.arguments[3];
                        const float endAngle      = args.arguments[4];
                        //TODO; Handle antiClockWise
                        //bool        antiClockWise = args.arguments[5];

                        const float width  = radius * 2;
                        const float height = radius * 2;

                        path.addArc(x, y, width, height, startAngle, endAngle);

                        return juce::var();
                    }
            });

            setProperty("quadraticCurveTo", juce::var::NativeFunction {
                    [=](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(graphics);
                        jassert(args.numArguments == 4);

                        const float controlPointX = args.arguments[0];
                        const float controlPointY = args.arguments[1];
                        const float endPointX     = args.arguments[2];
                        const float endPointY     = args.arguments[3];

                        path.quadraticTo(controlPointX, controlPointY, endPointX, endPointY);

                        return juce::var();
                    }
            });

            //TODO: Implement CanvasRenderingContext2D.bezierCurveTo(). Will use juce::Path::cubicTo

            setProperty("closePath", juce::var::NativeFunction {
                    [=](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(graphics);
                        jassert(args.numArguments == 0);

                        path.closeSubPath();

                        return juce::var();
                    }
            });

            setProperty("stroke", juce::var::NativeFunction {
                    [=](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(graphics);
                        jassert(args.numArguments == 0);

                        graphics->setColour(properties.strokeStyle.colour);
                        graphics->strokePath(path, juce::PathStrokeType((float)properties.lineWidth));

                        return juce::var();
                    }
            });

            setProperty("fill", juce::var::NativeFunction {
                    [=](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(graphics);
                        jassert(args.numArguments == 0);

                        graphics->setColour(properties.strokeStyle.colour);
                        graphics->strokePath(path, juce::PathStrokeType((float)properties.lineWidth));
                        graphics->setColour(properties.fillStyle.colour);
                        graphics->fillPath(path);

                        return juce::var();
                    }
            });

            // Transforms
            setProperty("rotate", juce::var::NativeFunction {
                    [=](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(graphics);
                        jassert(args.numArguments == 1);

                        const float angle = args.arguments[0];

                        auto transform = juce::AffineTransform::rotation(angle);
                        graphics->addTransform(transform);
                        transformStack.push_back(transform.inverted());

                        return juce::var();
                    }
            });

            setProperty("translate", juce::var::NativeFunction {
                    [=](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(graphics);
                        jassert(args.numArguments == 2);

                        const float x = args.arguments[0];
                        const float y = args.arguments[1];

                        auto transform = juce::AffineTransform::translation(x, y);
                        graphics->addTransform(transform);
                        transformStack.push_back(transform.inverted());

                        return juce::var();
                    }
            });

            setProperty("setTransform", juce::var::NativeFunction {
                    [=](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(graphics);
                        jassert(args.numArguments == 6);

                        const float mat00 = args.arguments[0];
                        const float mat01 = args.arguments[1];
                        const float mat02 = args.arguments[2];
                        const float mat10 = args.arguments[3];
                        const float mat11 = args.arguments[4];
                        const float mat12 = args.arguments[5];

                        auto transform = juce::AffineTransform(mat00, mat01, mat02, mat10, mat11, mat12);
                        graphics->addTransform(transform);
                        transformStack.push_back(transform.inverted());

                        return juce::var();
                    }
            });

            setProperty("resetTransform", juce::var::NativeFunction {
                    [=](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(graphics);
                        jassert(args.numArguments == 0);

                        while (!transformStack.empty())
                        {
                            graphics->addTransform(transformStack.back());
                            transformStack.pop_back();
                        }

                        transformStack.clear();

                        return juce::var();
                    }
            });
        }

        //==============================================================================
        juce::Image                        image;
        std::unique_ptr<juce::Graphics>    graphics;
        juce::Path                         path;
        Properties                         properties;
        std::vector<juce::AffineTransform> transformStack;
    };

    //==============================================================================
    /** The CanvasView class is a core view for manual drawing routines within Blueprint's
        layout system via a CanvasContext.
     */
    class CanvasView : public View, public juce::Timer
    {
    public:
        //==============================================================================
        CanvasView()
            : ctx(new CanvasContext())
        {
        }

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

            jassert(ctx);
            ctx->init();

            if (props.contains("onDraw") && props["onDraw"].isMethod())
            {
                std::vector<juce::var> jsArgs {{ctx.get()}};
                juce::var::NativeFunctionArgs nfArgs (juce::var(), jsArgs.data(), jsArgs.size());

                std::invoke(props["onDraw"].getNativeFunction(), nfArgs);
            }
            else
            {
                DBG("You appear to have a Canvas element without an onDraw property in your js bundle.");
            }

            g.drawImageAt(ctx->getImage(), 0, 0);
        }

        void resized() override
        {
            ctx->setSize(getWidth(), getHeight());
        }

    private:
        //==============================================================================
        juce::ReferenceCountedObjectPtr<CanvasContext> ctx;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CanvasView)
    };

}
