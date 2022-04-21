/*
  ==============================================================================

    EcmascriptEngine.cpp
    Created: 24 Oct 2019 3:08:39pm

  ==============================================================================
*/

#include "EcmascriptEngine.h"

#if REACTJUCE_USE_HERMES
    #include "EcmascriptEngine_Hermes.cpp"
#elif REACTJUCE_USE_DUKTAPE
    #include "EcmascriptEngine_Duktape.cpp"
#endif


namespace reactjuce
{

    //==============================================================================
    EcmascriptEngine::EcmascriptEngine()
        : mPimpl(std::make_unique<Pimpl>())
    {
        /** If you hit this, you're probably trying to run a console application.

            Please make use of juce::ScopedJuceInitialiser_GUI because this JS engine requires event loops.
            Without the initialiser, the console app would always crash on exit,
            and things will probably not get cleaned up.
        */
        jassert (juce::MessageManager::getInstanceWithoutCreating() != nullptr);
    }

    EcmascriptEngine::~EcmascriptEngine()
    {
    }

    //==============================================================================
    juce::var EcmascriptEngine::evaluateInline (const juce::String& code)
    {
        return mPimpl->evaluateInline(code);
    }

    juce::var EcmascriptEngine::evaluate (const juce::File& code)
    {
        return mPimpl->evaluate(code);
    }

    juce::var EcmascriptEngine::evaluateBytecode (const juce::File &code)
    {
        return mPimpl->evaluateBytecode(code);
    }

    //==============================================================================
    void EcmascriptEngine::registerNativeMethod (const juce::String& name, juce::var::NativeFunction fn)
    {
        registerNativeProperty(name, juce::var(fn));
    }

    void EcmascriptEngine::registerNativeMethod (const juce::String& target, const juce::String& name, juce::var::NativeFunction fn)
    {
        registerNativeProperty(target, name, juce::var(fn));
    }

    //==============================================================================
    void EcmascriptEngine::registerNativeProperty (const juce::String& name, const juce::var& value)
    {
        mPimpl->registerNativeProperty(name, value);
    }

    void EcmascriptEngine::registerNativeProperty (const juce::String& target, const juce::String& name, const juce::var& value)
    {
        mPimpl->registerNativeProperty(target, name, value);
    }

    //==============================================================================
    juce::var EcmascriptEngine::invoke (const juce::String& name, const std::vector<juce::var>& vargs)
    {
        return mPimpl->invoke(name, vargs);
    }

    void EcmascriptEngine::reset()
    {
        mPimpl->reset();
    }

    //==============================================================================
    void EcmascriptEngine::debuggerAttach()
    {
        mPimpl->debuggerAttach();
    }

    void EcmascriptEngine::debuggerDetach()
    {
        mPimpl->debuggerDetach();
    }

    //==============================================================================
    SourceMap::SourceMap(const juce::String& source, const juce::String& map)
        : mapLoaded(false)
        , sourcePath(source)
    {
        mapLoaded = LoadMap(map);
    }

    SourceMap::SourceMap(const juce::String& source, const juce::File& map)
        : mapLoaded(false)
        , sourcePath(source)
    {
        if (map.existsAsFile())
            mapLoaded = LoadMap(map.loadFileAsString());
    }

    SourceMap::Location SourceMap::translate(int line, int col) const
    {
        // line and col are 1-based.
        SourceMap::Location failRes{sourcePath, line, col};
        if (!mapLoaded || line <= 0 || col <= 0 || static_cast<std::size_t>(line) > mappings.size())
            return failRes;

        const Segment* use_segment = nullptr;
        for (const auto& segment : mappings[line - 1])
            if (segment.segStartCol >= col)
                break;
            else
                use_segment = &segment;

        if (!use_segment || !use_segment->gotSourceMap || use_segment->origSourceId < 0 ||
            static_cast<std::size_t>(use_segment->origSourceId) >= sources.size())
            return failRes;

        return
            {
                sources[use_segment->origSourceId],
                use_segment->origStartLine + 1,
                col - use_segment->segStartCol + use_segment->origStartCol
            };
    }

    bool SourceMap::LoadMap(const juce::String& map)
    {
        auto json = juce::JSON::parse(map);
        const auto fileVersion = json.getProperty("version", juce::var());
        const auto fileMappings = json.getProperty("mappings", juce::var());
        auto fileSources = json.getProperty("sources", juce::var());

        if (!fileVersion.isInt() || static_cast<int>(fileVersion) != 3 ||
            !fileMappings.isString() || !fileSources.isArray() )
            return false;

        for (auto &i : *fileSources.getArray())
        {
            if (!i.isString())
                return false;
            sources.emplace_back(std::move(i.toString()));
        }

        bool firstSegmentInLine = true;
        std::vector<Segment> lineSegments;
        Segment segment;
        Segment lastSegment;
        int segmentIntNo = 0;
        int segmentIntVal = 0;
        int segmentIntCharNo = 0;

        for (auto p = fileMappings.toString().toRawUTF8(); *p; ++p)
        {
            if (*p == ',' || *p == ';')
            {
                segmentIntNo = segmentIntVal = segmentIntCharNo = 0;
                if (firstSegmentInLine)
                    firstSegmentInLine = false;
                else
                    segment.segStartCol += lastSegment.segStartCol;
                segment.origSourceId += lastSegment.origSourceId;
                segment.origStartLine += lastSegment.origStartLine;
                segment.origStartCol += lastSegment.origStartCol;
                lineSegments.push_back(segment);
                lastSegment = segment;
                segment = Segment();

                if (*p == ';')
                {
                    mappings.emplace_back(std::move(lineSegments));
                    firstSegmentInLine = true;
                }
            }
            else
            {
                static const std::string base64_charset =
                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                    "abcdefghijklmnopqrstuvwxyz"
                    "0123456789+/";
                auto i = base64_charset.find(*p);
                if (i == std::string::npos)
                    return false;
                segmentIntVal |= ((i & 0x1f) << (segmentIntCharNo * 5));
                if (i & 0x20)
                    ++segmentIntCharNo;
                else
                {
                    bool neg = (segmentIntVal & 1);
                    segmentIntVal >>= 1;
                    if (neg)
                        segmentIntVal = -segmentIntVal;
                    switch(segmentIntNo++)
                    {
                    case 0:
                        segment.segStartCol = segmentIntVal;
                        break;

                    case 1:
                        segment.origSourceId = segmentIntVal;
                        segment.gotSourceMap = true;
                        break;

                    case 2:
                        segment.origStartLine = segmentIntVal;
                        break;

                    case 3:
                        segment.origStartCol = segmentIntVal;
                        break;
                    }
                    segmentIntVal = segmentIntCharNo = 0;
                }
            }
        }
        // In case there's no final terminator.
        mappings.emplace_back(std::move(lineSegments));

        return true;
    }
}
