/*
  ==============================================================================

    CanvasView.cpp
    Created: 23 Jan 2021 21:45:00pm

  ==============================================================================
*/

#include "CanvasView.h"

namespace reactjuce
{
    namespace
    {
        void setFillStyle(CanvasView::CanvasContext           &ctx,
                          juce::Graphics                      &g,
                          const juce::var::NativeFunctionArgs &args)
        {
            jassert(args.numArguments == 1);
            juce::ignoreUnused(g);

            //TODO: Implement fillStyle pattern
            //TODO: Implement fillStyle gradient

            const auto colourHex = args.arguments[0].toString();
            ctx.properties.fillStyle.setColour(juce::Colour::fromString(colourHex));
        }

        void setStrokeStyle(CanvasView::CanvasContext           &ctx,
                            juce::Graphics                      &g,
                            const juce::var::NativeFunctionArgs &args)
        {
            jassert(args.numArguments == 1);
            juce::ignoreUnused(g);

            //TODO: Implement strokeStyle pattern
            //TODO: Implement stokeStyle gradient

            const auto colourHex = args.arguments[0].toString();
            ctx.properties.strokeStyle.setColour(juce::Colour::fromString(colourHex));
        }

        void setLineWidth(CanvasView::CanvasContext           &ctx,
                          juce::Graphics                      &g,
                          const juce::var::NativeFunctionArgs &args)
        {
            jassert(args.numArguments == 1);
            juce::ignoreUnused(g);

            const int lineWidth = args.arguments[0];
            ctx.properties.lineWidth = lineWidth;
        }

        /**
         * TODO: Should this be pushed out into some sort of generic helpers namespace for parsing
         *       CSS like props?
         *
         *       i.e juce::Font parseCSSFontString(const juce::String &font);
         *
         *
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
        void setFont(CanvasView::CanvasContext          &ctx,
                    juce::Graphics                      &g,
                    const juce::var::NativeFunctionArgs &args)
        {
            jassert(args.numArguments == 1);
            juce::ignoreUnused(g);

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

            ctx.properties.font = juce::Font(typeface, fontSize, flags);
        }

        void setTextAlign(CanvasView::CanvasContext           &ctx,
                          juce::Graphics                      &g,
                          const juce::var::NativeFunctionArgs &args)
        {
            jassert(args.numArguments == 1);
            juce::ignoreUnused(g);

            const juce::String textAlign = args.arguments[0].toString();

            //TODO: Have "start" and "end" depend on a "direction" property.
            //      Leaving with sensible defaults for now. No clear way
            //      to provide/infer text direction from locale at the moment etc.
            if (textAlign == "left")
                ctx.properties.textAlign = juce::Justification::left;
            else if (textAlign == "right")
                ctx.properties.textAlign = juce::Justification::right;
            else if (textAlign == "center")
                ctx.properties.textAlign = juce::Justification::horizontallyCentred;
            else if (textAlign == "start")
                ctx.properties.textAlign = juce::Justification::left;
            else if (textAlign == "end")
                ctx.properties.textAlign = juce::Justification::right;
        }

        /** void rect(CanvasView::CanvasContext &ctx, juce::Graphics &g, const juce::var::NativeFunctionArgs &args) */
        /** { */
        /**     jassert(args.numArguments == 4); */

        /**     const float x = args.arguments[0]; */
        /**     const float y = args.arguments[1]; */
        /**     const float width = args.arguments[2]; */
        /**     const float height = args.arguments[3]; */

        /**     // TODO: For some reason this operation closes the current path. */
        /**     //       The API docs suggest addRectangle should be added as a new sub-path */
        /**     //       leaving the previous path open. */
        /**     //       Fix. */
        /**     ctx.path.addRectangle(x, y, width, height); */
        /** } */

        void fillRect(CanvasView::CanvasContext           &ctx,
                      juce::Graphics                      &g,
                      const juce::var::NativeFunctionArgs &args)
        {
            jassert(args.numArguments == 4);

            const int x      = args.arguments[0];
            const int y      = args.arguments[1];
            const int width  = args.arguments[2];
            const int height = args.arguments[3];

            g.setFillType(ctx.properties.fillStyle);
            g.fillRect(x, y, width, height);
        }

        void strokeRect(CanvasView::CanvasContext           &ctx,
                        juce::Graphics                      &g,
                        const juce::var::NativeFunctionArgs &args)
        {
            jassert(args.numArguments == 4);

            const int x = args.arguments[0];
            const int y = args.arguments[1];
            const int width = args.arguments[2];
            const int height = args.arguments[3];

            g.setColour(ctx.properties.strokeStyle.colour);
            g.drawRect(x, y, width, height, ctx.properties.lineWidth);
        }

        void strokeRoundedRect(CanvasView::CanvasContext           &ctx,
                               juce::Graphics                      &g,
                               const juce::var::NativeFunctionArgs &args)
        {
            jassert(args.numArguments == 5);

            const float x = args.arguments[0];
            const float y = args.arguments[1];
            const float width = args.arguments[2];
            const float height = args.arguments[3];
            const float cornerSize = args.arguments[4];

            g.setColour(ctx.properties.strokeStyle.colour);

            g.drawRoundedRectangle(x,
                                   y,
                                   width,
                                   height,
                                   cornerSize,
                                   (float)ctx.properties.lineWidth);
        }

        void fillRoundedRect(CanvasView::CanvasContext           &ctx,
                             juce::Graphics                      &g,
                             const juce::var::NativeFunctionArgs &args)
        {
            jassert(args.numArguments == 5);

            const float x = args.arguments[0];
            const float y = args.arguments[1];
            const float width = args.arguments[2];
            const float height = args.arguments[3];
            const float cornerSize = args.arguments[4];

            g.setFillType(ctx.properties.fillStyle);
            g.fillRoundedRectangle(x, y, width, height, cornerSize);
        }

        void clearRect(CanvasView::CanvasContext           &ctx,
                       juce::Graphics                      &g,
                       const juce::var::NativeFunctionArgs &args)
        {
            jassert(args.numArguments == 4);
            juce::ignoreUnused(ctx);

            const int x = args.arguments[0];
            const int y = args.arguments[1];
            const int width = args.arguments[2];
            const int height = args.arguments[3];

            juce::Rectangle<int> area (x, y, width, height);

            // Set fill color to transparent black
            g.setColour({});
            g.fillRect(area);
        }

        void beginPath(CanvasView::CanvasContext           &ctx,
                       juce::Graphics                      &g,
                       const juce::var::NativeFunctionArgs &args)
        {
            jassert(args.numArguments == 0);
            juce::ignoreUnused(args);
            juce::ignoreUnused(g);

            // Reset the current path on a call to beginPath on the context.
            ctx.path = juce::Path();
        }

        void lineTo(CanvasView::CanvasContext           &ctx,
                    juce::Graphics                      &g,
                    const juce::var::NativeFunctionArgs &args)
        {
            jassert(args.numArguments == 2);
            juce::ignoreUnused(g);

            const float x = args.arguments[0];
            const float y = args.arguments[1];

            ctx.path.lineTo(x, y);
        }

        void moveTo(CanvasView::CanvasContext           &ctx,
                    juce::Graphics                      &g,
                    const juce::var::NativeFunctionArgs &args)
        {
            jassert(args.numArguments == 2);
            juce::ignoreUnused(g);

            const float x = args.arguments[0];
            const float y = args.arguments[1];

            ctx.path.startNewSubPath(x, y);
        }

        void arc(CanvasView::CanvasContext           &ctx,
                 juce::Graphics                      &g,
                 const juce::var::NativeFunctionArgs &args)
        {
            // CanvasRenderingContext2D.arc() uses default antiClockWise false arg
            jassert(args.numArguments >= 5 && args.numArguments <= 6);
            juce::ignoreUnused(g);

            const float x             = args.arguments[0];
            const float y             = args.arguments[1];
            const float radius        = args.arguments[2];
            const float startAngle    = args.arguments[3];
            const float endAngle      = args.arguments[4];
            //TODO; Handle antiClockWise
            //bool        antiClockWise = args.arguments[5];

            ctx.path.addCentredArc(x, y, radius, radius, 0.0f, startAngle, endAngle, false);
        }

        void quadraticCurveTo(CanvasView::CanvasContext           &ctx,
                              juce::Graphics                      &g,
                              const juce::var::NativeFunctionArgs &args)
        {
            jassert(args.numArguments == 4);
            juce::ignoreUnused(g);

            const float controlPointX = args.arguments[0];
            const float controlPointY = args.arguments[1];
            const float endPointX     = args.arguments[2];
            const float endPointY     = args.arguments[3];

            ctx.path.quadraticTo(controlPointX, controlPointY, endPointX, endPointY);
        }

        void closePath(CanvasView::CanvasContext           &ctx,
                       juce::Graphics                      &g,
                       const juce::var::NativeFunctionArgs &args)
        {
            jassert(args.numArguments == 0);
            juce::ignoreUnused(args);
            juce::ignoreUnused(g);

            ctx.path.closeSubPath();
        }

        void stroke(CanvasView::CanvasContext           &ctx,
                    juce::Graphics                      &g,
                    const juce::var::NativeFunctionArgs &args)
        {
            jassert(args.numArguments == 0);
            juce::ignoreUnused(args);

            g.setColour(ctx.properties.strokeStyle.colour);
            g.strokePath(ctx.path, juce::PathStrokeType((float)ctx.properties.lineWidth));
        }

        void fill(CanvasView::CanvasContext           &ctx,
                  juce::Graphics                      &g,
                  const juce::var::NativeFunctionArgs &args)
        {
            jassert(args.numArguments == 0);
            juce::ignoreUnused(args);

            g.setColour(ctx.properties.fillStyle.colour);
            g.fillPath(ctx.path);
        }

        void rotate(CanvasView::CanvasContext           &ctx,
                    juce::Graphics                      &g,
                    const juce::var::NativeFunctionArgs &args)
        {
            jassert(args.numArguments == 1);

            const float angle = args.arguments[0];

            auto transform = juce::AffineTransform::rotation(angle);
            g.addTransform(transform);
            ctx.transformStack.push_back(transform.inverted());
        }

        void translate(CanvasView::CanvasContext           &ctx,
                       juce::Graphics                      &g,
                       const juce::var::NativeFunctionArgs &args)
        {
            jassert(args.numArguments == 2);

            const float x = args.arguments[0];
            const float y = args.arguments[1];

            auto transform = juce::AffineTransform::translation(x, y);
            g.addTransform(transform);
            ctx.transformStack.push_back(transform.inverted());
        }

        void setTransform(CanvasView::CanvasContext           &ctx,
                          juce::Graphics                      &g,
                          const juce::var::NativeFunctionArgs &args)
        {
            jassert(args.numArguments == 6);

            const float mat00 = args.arguments[0];
            const float mat01 = args.arguments[1];
            const float mat02 = args.arguments[2];
            const float mat10 = args.arguments[3];
            const float mat11 = args.arguments[4];
            const float mat12 = args.arguments[5];

            auto transform = juce::AffineTransform(mat00, mat01, mat02, mat10, mat11, mat12);
            g.addTransform(transform);
            ctx.transformStack.push_back(transform.inverted());
        }

        void resetTransform(CanvasView::CanvasContext           &ctx,
                            juce::Graphics                      &g,
                            const juce::var::NativeFunctionArgs &args)
        {
            jassert(args.numArguments == 0);
            juce::ignoreUnused(args);

            while (!ctx.transformStack.empty())
            {
                g.addTransform(ctx.transformStack.back());
                ctx.transformStack.pop_back();
            }

            ctx.transformStack.clear();
        }

        void drawImage(CanvasView::CanvasContext           &ctx,
                       juce::Graphics                      &g,
                       const juce::var::NativeFunctionArgs &args)
        {
            jassert(args.numArguments >= 3 && args.numArguments <= 5);
            juce::ignoreUnused(ctx);

            const juce::String svg  = args.arguments[0].toString();
            const float        xPos = args.arguments[1];
            const float        yPos = args.arguments[2];

            //TODO: Add support for drawimage source width and source height to draw sub rect of an image.
            //      ctx.drawImage(image, sx, sy, sWidth, sHeight, dx, dy, dWidth, dHeight);

            std::unique_ptr<juce::XmlElement> svgElement(juce::XmlDocument::parse(svg));

            if (!svgElement)
            {
                DBG("\"WARNING: Invalid SVG string supplied to `drawImage`.\"");
                return;
            }

            std::unique_ptr<juce::Drawable> svgDrawable(juce::Drawable::createFromSVG(*svgElement));

            if (args.numArguments == 5)
            {
                const float destWidth  = args.arguments[3];
                const float destHeight = args.arguments[4];
                const auto  bounds     = juce::Rectangle<float>(xPos, yPos, destWidth, destHeight);

                svgDrawable->setTransformToFit(bounds, juce::RectanglePlacement::stretchToFit);
                svgDrawable->draw(g, 1.0f);
            }
            else
            {
                svgDrawable->drawAt(g, xPos, yPos, 1.0);
            }
        }

        void strokeText(CanvasView::CanvasContext           &ctx,
                        juce::Graphics                      &g,
                        const juce::var::NativeFunctionArgs &args)
        {
            jassert(args.numArguments >= 3 && args.numArguments <= 4);

            const juce::String text = args.arguments[0].toString();
            const float        xPos = args.arguments[1];
            const float        yPos = args.arguments[2];

            // Default maxLineWidth to full context width
            float maxLineWidth = (float)ctx.width;

            if (args.numArguments == 4)
                maxLineWidth = args.arguments[3];

            juce::Path textPath;
            juce::GlyphArrangement glyphArrangement;

            glyphArrangement.addJustifiedText(ctx.properties.font,
                                              text,
                                              xPos,
                                              yPos,
                                              maxLineWidth,
                                              ctx.properties.textAlign);

            glyphArrangement.createPath(textPath);

            g.setColour(ctx.properties.strokeStyle.colour);
            g.strokePath(textPath, juce::PathStrokeType((float)ctx.properties.lineWidth));
        }

        void fillText(CanvasView::CanvasContext           &ctx,
                      juce::Graphics                      &g,
                      const juce::var::NativeFunctionArgs &args)
        {
            jassert(args.numArguments >= 3 && args.numArguments <= 4);

            const juce::String text = args.arguments[0].toString();
            const float        xPos = args.arguments[1];
            const float        yPos = args.arguments[2];

            // Default maxLineWidth to full context width
            float maxLineWidth = (float)ctx.width;

            if (args.numArguments == 4)
                maxLineWidth = args.arguments[3];

            juce::Path textPath;
            juce::GlyphArrangement glyphArrangement;

            glyphArrangement.addJustifiedText(ctx.properties.font,
                                              text,
                                              xPos,
                                              yPos,
                                              maxLineWidth,
                                              ctx.properties.textAlign);

            glyphArrangement.createPath(textPath);

            g.setFillType(ctx.properties.fillStyle);
            g.fillPath(textPath);
        }

        using DrawCommand = std::function<void(CanvasView::CanvasContext&,
                                               juce::Graphics&,
                                               const juce::var::NativeFunctionArgs&)>;

        std::unordered_map<juce::String, DrawCommand> DrawCommands
        {
            { "setFillStyle"      , setFillStyle      },
            { "setStrokeStyle"    , setStrokeStyle    },
            { "setLineWidth"      , setLineWidth      },
            { "setFont"           , setFont           },
            { "setTextAlign"      , setTextAlign      },
            { "fillRect"          , fillRect          },
            { "strokeRect"        , strokeRect        },
            { "strokeRoundedRect" , strokeRoundedRect },
            { "fillRoundedRect"   , fillRoundedRect   },
            { "clearRect"         , clearRect         },
            { "beginPath"         , beginPath         },
            { "lineTo"            , lineTo            },
            { "moveTo"            , moveTo            },
            { "arc"               , arc               },
            { "quadraticCurveTo"  , quadraticCurveTo  },
            { "closePath"         , closePath         },
            { "stroke"            , stroke            },
            { "fill"              , fill              },
            { "rotate"            , rotate            },
            { "translate"         , translate         },
            { "setTransform"      , setTransform      },
            { "resetTransform"    , resetTransform    },
            { "drawImage"         , drawImage         },
            { "strokeText"        , strokeText        },
            { "fillText"          , fillText          },
        };
    }

    //==============================================================================
    void CanvasView::processDrawCommands(CanvasContext  &ctx,
                                        juce::Graphics  &g,
                                        const juce::var &drawCommands)
    {
        jassert(drawCommands.isArray());

        for (juce::var &command : *drawCommands.getArray())
        {
            jassert(command.isArray());
            jassert(command.getArray()->size() >= 1);

            const auto args = command.getArray();
            const auto name = (*args)[0];

            if (auto it = DrawCommands.find(name); it != DrawCommands.end())
            {
                it->second(ctx,
                           g,
                           juce::var::NativeFunctionArgs(juce::var(),
                                                         args->getRawDataPointer() + 1,
                                                         args->size() - 1));
            }
        }
    }

    //==============================================================================
    CanvasView::CanvasView()
        : canvasImage(juce::Image::ARGB, 1, 1, true)
    {

    }

    CanvasView::~CanvasView()
    {
        if (isTimerRunning())
            stopTimer();
    }

    //==============================================================================
    void CanvasView::setProperty(const juce::Identifier& name, const juce::var& value)
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
        
        if (name == statefulProp && props[statefulProp])
        {
            resized();
        }
    }

    //==============================================================================
    void CanvasView::timerCallback()
    {
        repaint();
    }

    //==============================================================================
    void CanvasView::paint (juce::Graphics& g)
    {
        View::paint(g);

        CanvasContext ctx = { getLocalBounds().getWidth(), getLocalBounds().getHeight() };

        if (props.contains(onDrawProp) && props[onDrawProp].isMethod())
        {
            const auto drawCommands = std::invoke( props[onDrawProp].getNativeFunction()
                                                 , juce::var::NativeFunctionArgs(juce::var(), nullptr, 0u));

            if (props.contains(statefulProp) && props[statefulProp])
            {
                juce::Graphics imageGraphics(canvasImage);
                processDrawCommands(ctx, imageGraphics, drawCommands);
                g.drawImageAt(canvasImage, 0, 0);
            }
            else
            {
                processDrawCommands(ctx, g, drawCommands);
            }
        }
        else
        {
            DBG("You appear to have a Canvas element without an onDraw property in your js bundle.");
        }
    }

    void CanvasView::resized()
    {
        View::resized();

        if (props.contains(statefulProp) && props[statefulProp])
        {
            //TODO: Fix image scalling for retina displays.
            //      May require passing an optional scaleFactor
            //      arg through to draw commands.
            const auto bounds = getLocalBounds();
            if (bounds.getWidth() > 0 && bounds.getHeight() > 0)
            {
                canvasImage = canvasImage.rescaled(bounds.getWidth(),
                                                   bounds.getHeight(),
                                                   juce::Graphics::highResamplingQuality);
            }
        }
    }

    //==============================================================================
}

