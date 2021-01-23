/*
  ==============================================================================

    CanvasView.h
    Created: 19 May 2020 9:02:25pm

  ==============================================================================
*/

#pragma once

#include "View.h"


namespace reactjuce
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
            FillStyle           fillStyle{};
            StrokeStyle         strokeStyle{};
            int                 lineWidth = 1;
            juce::Font          font{};
            juce::Justification textAlign = juce::Justification::left;
        };

        explicit CanvasContext()
            : ctxWidth(1)
            , ctxHeight(1)
            , image(juce::Image::ARGB, ctxWidth, ctxHeight, true)
        {
            registerNativeProperties();
            registerNativeFunctions();
        }

        ~CanvasContext() = default;

        /** Set the size of the CanvasContext. This is akin to setting the width and height attributes
         *  on a HTML <canvas> element. If you have a View or a juce::Component that wished
         *  to render a CanvasContext you should ensure CanvasContext::setSize() is called from the
         *  View/Component's resized function.
         */
        void setSize(int width, int height)
        {
            ctxWidth  = std::max(width, 1);
            ctxHeight = std::max(height, 1);
            image     = image.rescaled(ctxWidth, ctxHeight, juce::Graphics::highResamplingQuality);
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
            graphics   = std::make_unique<juce::Graphics>(image);
            properties = Properties{};
            path       = juce::Path{};

            transformStack.clear();
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

            /**
             * Currently supports space separated css-font like strings with the below  supported format/properties:
             *
             *  "<font-style> <font-weight> <font-size> <font-family>"
             *
             *  One of the following values may be supplied for each font property.
             *
             *  <font-style>:  italic, normal
             *  <font-weight>: bold, normal
             *  <font-size>:   font size in pixels, i.e. "14px"
             *  <font-family>: font name representing an available font/typeface on the system, i.e. "sans-serif"
             *
             *  Note, font-size and font-family must be supplied in the string from JS but font-style and font-weight
             *  are optional, though they must precede font-size and font-family and be ordered font-style, font-weight.
             *  If no values are supplied for font-style and font-weight the juce::Font::plain flag will be used.
             *
             *  Example font strings from JS bundle:
             *
             *  'italic bold 14px sans-serif'
             *  'bold 14px sans-serif'
             *  'italic 14px sans-serif'
             *  '14px sans-serif'
             *
             *  Note, multi-word/space-separated strings should be place in quotes:
             *
             *  'bold 14px "DejaVu Serif"'
             * */
            setProperty("__setFont", juce::var::NativeFunction {
                    [=](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(graphics);
                        jassert(args.numArguments == 1);

                        auto fontString = args.arguments[0].toString();
                        auto values = juce::StringArray::fromTokens (fontString, juce::StringRef (" "), {});

                        jassert(values.size() >=2 && values.size() <= 4);

                        juce::Font::FontStyleFlags flags    = juce::Font::plain;
                        float                      fontSize = 0;
                        juce::String               typeface;

                        for (auto& value : values)
                        {
                            // Remove any quoted values likely used when specifying fonts
                            value = value.unquoted();

                            if (value == "bold")
                            {
                                flags = static_cast<juce::Font::FontStyleFlags>(flags | juce::Font::bold);
                            }
                            else if (value == "italic")
                            {
                                flags = static_cast<juce::Font::FontStyleFlags>(flags | juce::Font::italic);
                            }
                            else if (value.contains("px"))
                            {
                                fontSize = (float)value.getIntValue();
                            }
                            else if (value != "normal")
                            {
                                typeface = value;
                            }
                        }

                        properties.font = juce::Font(typeface, fontSize, flags);

                        return juce::var();
                    }
            });

            setProperty("__setTextAlign", juce::var::NativeFunction {
                    [=](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(graphics);
                        jassert(args.numArguments == 1);

                        const juce::String textAlign = args.arguments[0].toString();

                        //TODO: Have "start" and "end" depend on a "direction" property.
                        //      Leaving with sensible defaults for now. No clear way
                        //      to provide/infer text direction from locale at the moment etc.
                        if (textAlign == "left")
                            properties.textAlign = juce::Justification::left;
                        else if (textAlign == "right")
                            properties.textAlign = juce::Justification::right;
                        else if (textAlign == "center")
                            properties.textAlign = juce::Justification::horizontallyCentred;
                        else if (textAlign == "start")
                            properties.textAlign = juce::Justification::left;
                        else if (textAlign == "end")
                            properties.textAlign = juce::Justification::right;

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

                        graphics->setFillType(properties.fillStyle);
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

                        graphics->setColour(properties.strokeStyle.colour);
                        graphics->drawRect(x, y, width, height, properties.lineWidth);

                        return juce::var();
                    }
            });

            setProperty("strokeRoundedRect", juce::var::NativeFunction {
                    [=](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(graphics);
                        jassert(args.numArguments == 5);

                        const float x = args.arguments[0];
                        const float y = args.arguments[1];
                        const float width = args.arguments[2];
                        const float height = args.arguments[3];
                        const float cornerSize = args.arguments[4];

                        graphics->setColour(properties.strokeStyle.colour);
                        graphics->drawRoundedRectangle(x, y, width, height, cornerSize, (float) properties.lineWidth);

                        return juce::var();
                    }
            });

            setProperty("fillRoundedRect", juce::var::NativeFunction {
                    [=](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(graphics);
                        jassert(args.numArguments == 5);

                        const float x = args.arguments[0];
                        const float y = args.arguments[1];
                        const float width = args.arguments[2];
                        const float height = args.arguments[3];
                        const float cornerSize = args.arguments[4];

                        graphics->setFillType(properties.fillStyle);
                        graphics->fillRoundedRectangle(x, y, width, height, cornerSize);

                        return juce::var();
                    }
            });

            setProperty("clearRect", juce::var::NativeFunction {
                    [=](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(graphics);
                        jassert(args.numArguments == 4);

                        const int x = args.arguments[0];
                        const int y = args.arguments[1];
                        const int width = args.arguments[2];
                        const int height = args.arguments[3];

                        juce::Rectangle<int> area (x, y, width, height);
                        image.clear(area);

                        return juce::var();
                    }
            });

            // Path functions
            setProperty("beginPath", juce::var::NativeFunction {
                    [=](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(graphics);
                        jassert(args.numArguments == 0);
                        juce::ignoreUnused(args);

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
                        juce::ignoreUnused(args);

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

                        path.addCentredArc(x, y, radius, radius, 0.0f, startAngle, endAngle, false);
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
                        juce::ignoreUnused(args);

                        path.closeSubPath();

                        return juce::var();
                    }
            });

            setProperty("stroke", juce::var::NativeFunction {
                    [=](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(graphics);
                        jassert(args.numArguments == 0);
                        juce::ignoreUnused(args);

                        graphics->setColour(properties.strokeStyle.colour);
                        graphics->strokePath(path, juce::PathStrokeType((float)properties.lineWidth));

                        return juce::var();
                    }
            });

            setProperty("fill", juce::var::NativeFunction {
                    [=](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(graphics);
                        jassert(args.numArguments == 0);
                        juce::ignoreUnused(args);

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
                        juce::ignoreUnused(args);

                        while (!transformStack.empty())
                        {
                            graphics->addTransform(transformStack.back());
                            transformStack.pop_back();
                        }

                        transformStack.clear();

                        return juce::var();
                    }
            });

            // drawImage support
            setProperty("drawImage", juce::var::NativeFunction {
                    [=](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(graphics);
                        jassert(args.numArguments >= 3 && args.numArguments <= 5);

                        const juce::String svg  = args.arguments[0].toString();
                        const float        xPos = args.arguments[1];
                        const float        yPos = args.arguments[2];

                        //TODO: Add support for drawimage source width and source height to draw sub rect of an image.
                        //      ctx.drawImage(image, sx, sy, sWidth, sHeight, dx, dy, dWidth, dHeight);

                        std::unique_ptr<juce::XmlElement> svgElement(juce::XmlDocument::parse(svg));

                        if (!svgElement)
                        {
                            DBG("\"WARNING: Invalid SVG string supplied to `drawImage`.\"");
                            return juce::var();
                        }

                        std::unique_ptr<juce::Drawable> svgDrawable(juce::Drawable::createFromSVG(*svgElement));

                        if (args.numArguments == 5)
                        {
                            const float destWidth  = args.arguments[3];
                            const float destHeight = args.arguments[4];
                            const auto  bounds     = juce::Rectangle<float>(xPos, yPos, destWidth, destHeight);

                            svgDrawable->setTransformToFit(bounds, juce::RectanglePlacement::stretchToFit);
                            svgDrawable->draw(*graphics, 1.0f);
                        }
                        else
                        {
                            svgDrawable->drawAt(*graphics, xPos, yPos, 1.0);
                        }

                        return juce::var();
                    }
            });

            // Text functions
            setProperty("strokeText", juce::var::NativeFunction {
                    [=](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(graphics);
                        jassert(args.numArguments >= 3 && args.numArguments <= 4);

                        const juce::String text = args.arguments[0].toString();
                        const float        xPos = args.arguments[1];
                        const float        yPos = args.arguments[2];

                        // Default maxLineWidth to full context width
                        float maxLineWidth = (float)ctxWidth;

                        if (args.numArguments == 4)
                            maxLineWidth = args.arguments[3];

                        juce::Path textPath;
                        juce::GlyphArrangement glyphArrangement;

                        glyphArrangement.addJustifiedText(properties.font, text, xPos, yPos, maxLineWidth, properties.textAlign);
                        glyphArrangement.createPath(textPath);

                        graphics->setColour(properties.strokeStyle.colour);
                        graphics->strokePath(textPath, juce::PathStrokeType((float)properties.lineWidth));

                        return juce::var();
                    }
            });

            setProperty("fillText", juce::var::NativeFunction {
                    [=](const juce::var::NativeFunctionArgs& args) -> juce::var {
                        jassert(graphics);
                        jassert(args.numArguments >= 3 && args.numArguments <= 4);

                        const juce::String text = args.arguments[0].toString();
                        const float        xPos = args.arguments[1];
                        const float        yPos = args.arguments[2];

                        // Default maxLineWidth to full context width
                        float maxLineWidth = (float)ctxWidth;

                        if (args.numArguments == 4)
                            maxLineWidth = args.arguments[3];

                        juce::Path textPath;
                        juce::GlyphArrangement glyphArrangement;

                        glyphArrangement.addJustifiedText(properties.font, text, xPos, yPos, maxLineWidth, properties.textAlign);
                        glyphArrangement.createPath(textPath);

                        graphics->setFillType(properties.fillStyle);
                        graphics->fillPath(textPath);

                        return juce::var();
                    }
            });
        }

        //==============================================================================
        int                                ctxWidth;
        int                                ctxHeight;

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
        static const inline juce::Identifier animateProp = "animate";
        static const inline juce::Identifier onDrawProp  = "onDraw";

        //==============================================================================
        CanvasView()
            : ctx(new CanvasContext())
        {
        }

        ~CanvasView() override
        {
            if (isTimerRunning())
                stopTimer();
        }

        //==============================================================================
        void setProperty (const juce::Identifier& name, const juce::var& value) override
        {
            View::setProperty(name, value);

            if (name == animateProp)
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

            if (props.contains(onDrawProp) && props[onDrawProp].isMethod())
            {
                std::vector<juce::var> jsArgs {{ctx.get()}};
                juce::var::NativeFunctionArgs nfArgs (juce::var(), jsArgs.data(), static_cast<int>(jsArgs.size()));

                std::invoke(props[onDrawProp].getNativeFunction(), nfArgs);
            }
            else
            {
                DBG("You appear to have a Canvas element without an onDraw property in your js bundle.");
            }

            g.drawImageAt(ctx->getImage(), 0, 0);
        }

        void resized() override
        {
            View::resized();
            ctx->setSize(getWidth(), getHeight());
        }

    private:
        //==============================================================================
        juce::ReferenceCountedObjectPtr<CanvasContext> ctx;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CanvasView)
    };

}
