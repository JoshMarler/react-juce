/*
  ==============================================================================

    CanvasView.h
    Created: 19 May 2020 9:02:25pm

  ==============================================================================
*/

#pragma once

#include"View.h"


namespace reactjuce
{
    //==============================================================================
    /**
     * The CanvasView class provide HTML canvas functionality to react-juce.
     * JS CanvasRenderingContext calls are handled here and converted to JUCE graphics
     * routines.
     */
    class CanvasView : public View, public juce::Timer
    {
    public:
        //==============================================================================
        static const inline juce::Identifier animateProp  = "animate";
        static const inline juce::Identifier onDrawProp   = "onDraw";
        static const inline juce::Identifier statefulProp = "stateful";

        //==============================================================================
        using FillStyle   = juce::FillType;
        using StrokeStyle = juce::FillType;

        struct CanvasContextProperties
        {
            FillStyle           fillStyle;
            StrokeStyle         strokeStyle;
            int                 lineWidth = 1;
            juce::Font          font;
            juce::Justification textAlign = juce::Justification::left;
        };

        struct CanvasContext
        {
            int                                width          = 1;
            int                                height         = 1;
            juce::Path                         path           = {};
            CanvasContextProperties            properties     = {};
            std::vector<juce::AffineTransform> transformStack = {};
        };

        //==============================================================================
        static void processDrawCommands(CanvasContext   &ctx,
                                        juce::Graphics  &g,
                                        const juce::var &drawCommands);

        //==============================================================================
        CanvasView();
        ~CanvasView() override;

        //==============================================================================
        void setProperty (const juce::Identifier& name, const juce::var& value) override;

        //==============================================================================
        void timerCallback() override;

        //==============================================================================
        void paint (juce::Graphics& g) override;
        void resized() override;

        //==============================================================================

    private:
        juce::Image canvasImage;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CanvasView)
    };

}


