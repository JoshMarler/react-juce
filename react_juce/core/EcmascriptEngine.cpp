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

}
