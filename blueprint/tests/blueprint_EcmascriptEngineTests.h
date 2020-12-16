#if ! DOXYGEN

namespace blueprint
{

//==============================================================================
class BlueprintUnitTest : public juce::UnitTest
{
public:
    BlueprintUnitTest (juce::StringRef name, juce::StringRef cat = "EcmascriptEngine")
        : juce::UnitTest ("Blueprint EcmascriptEngine - " + name, cat) {}

protected:
    blueprint::EcmascriptEngine engine;

    /** Shortcut to evaluate a script in the engine instance. */
    juce::var evaluate (const juce::String& code)               { return engine.evaluateInline (code); }
    /** Shortcut to evaluate a file as a script in the engine instance. */
    juce::var evaluate (const juce::File& script)               { return engine.evaluate (script); }

    /** Shortcut to the engine's invoke method. */
    template<typename... Args>
    juce::var invoke (const juce::String& name, Args... args)   { return engine.invoke (name, args...);  }

private:
    JUCE_DECLARE_NON_COPYABLE (BlueprintUnitTest)
};

//==============================================================================
class EcmascriptEngineEvaluateTest final : public BlueprintUnitTest
{
public:
    EcmascriptEngineEvaluateTest() : BlueprintUnitTest ("Script Evaluation") {}

    void runTest() override
    {
        beginTest ("Numbers");
        expect (static_cast<int> (evaluate ("2 + 3;")) == 5);
        expect (static_cast<int> (evaluate ("2 * 3;")) == 6);
        expect (static_cast<int> (evaluate ("Math.pow(2, 2);")) == 4);

        beginTest ("Strings");
        expect (evaluate ("['h', 'e', 'y'].join('');").toString() == "hey");
    }
};

//==============================================================================
class EcmascriptEngineNativeFunctionTest final : public BlueprintUnitTest
{
public:
    EcmascriptEngineNativeFunctionTest() : BlueprintUnitTest ("Native Function Interfaces") {}

    void runTest() override
    {
        beginTest ("Global Functions");

        engine.registerNativeMethod ("myMultiply",
        [&] (const juce::var::NativeFunctionArgs& args) -> juce::var
        {
            expect (args.numArguments == 2);
            return static_cast<int> (args.arguments[0]) * static_cast<int> (args.arguments[1]);
        });

        expect (static_cast<int> (evaluate ("myMultiply(2, 3);")) == 6);
        expect (static_cast<int> (evaluate ("this.myMultiply(2, 2);")) == 4);

        beginTest ("Namespaced Function");

        engine.registerNativeProperty ("Blueprint", juce::JSON::parse ("{}"));
        engine.registerNativeMethod ("Blueprint", "squareIt",
        [&](const juce::var::NativeFunctionArgs& args) -> juce::var
        {
            expect (args.numArguments == 1);
            return juce::square (static_cast<int> (args.arguments[0]));
        });

        expect (static_cast<int> (evaluate ("Blueprint.squareIt(2);")) == 4);
        expect (static_cast<int> (evaluate ("this.Blueprint.squareIt(3);")) == 9);
    }
};

//==============================================================================
class EcmascriptEngineNativePropertyTest final : public BlueprintUnitTest
{
public:
    EcmascriptEngineNativePropertyTest() : BlueprintUnitTest ("Native Property Interfaces") {}

    void runTest() override
    {
        beginTest ("Global Property");
        engine.registerNativeProperty ("BlueprintNative", juce::JSON::parse ("{}"));
        engine.registerNativeProperty ("DOUBLE_PI", juce::MathConstants<double>::twoPi);
        engine.registerNativeProperty ("PLUGIN_VERSION", "1.3.5");

        expect (static_cast<double> (evaluate ("DOUBLE_PI;")) == juce::MathConstants<double>::twoPi);
        expect (evaluate ("PLUGIN_VERSION").toString() == "1.3.5");
        expect (evaluate ("BlueprintNative").isObject());

        beginTest ("Namespaced Property");
        engine.registerNativeProperty ("BlueprintNative", "Constants", juce::JSON::parse ("{}"));
        engine.registerNativeProperty ("BlueprintNative.Constants", "PLUGIN_VERSION", "1.3.5");
        engine.registerNativeProperty ("BlueprintNative.Constants", "PLUGIN_NAME", "Temper");

        expect (evaluate ("BlueprintNative.Constants.PLUGIN_VERSION").toString() == "1.3.5");
        expect (evaluate ("BlueprintNative.Constants.PLUGIN_NAME").toString() == "Temper");

        auto constants = evaluate ("BlueprintNative.Constants");
        expect (constants.isObject());

        auto* obj = constants.getDynamicObject();
        expect (obj != nullptr);

        expect (obj->hasProperty ("PLUGIN_VERSION"));
        expect (obj->hasProperty ("PLUGIN_NAME"));
        expect (obj->getProperty ("PLUGIN_VERSION").toString() == "1.3.5");
        expect (obj->getProperty ("PLUGIN_NAME").toString() == "Temper");
    }
};

//==============================================================================
class EcmascriptEngineInvokeTest final : public BlueprintUnitTest
{
public:
    EcmascriptEngineInvokeTest() : BlueprintUnitTest ("Function Invocation Interfaces") {}

    void runTest() override
    {
        beginTest ("Invoking Builtins");
        expect (engine.invoke ("String.fromCharCode", 42).toString() == "*");
        expect (static_cast<int> (engine.invoke ("parseInt", 1.00031439, 10)) == 1);
        expect (static_cast<int> (engine.invoke ("Math.abs", -1)) == 1);

        beginTest ("Invoking Native Methods");
        engine.registerNativeProperty ("BlueprintNative", juce::JSON::parse ("{}"));

        engine.registerNativeMethod ("BlueprintNative", "squareIt",
        [&](const juce::var::NativeFunctionArgs& args)
        {
            return juce::square (static_cast<int> (args.arguments[0]));
        });

        expect (static_cast<int> (engine.invoke ("BlueprintNative.squareIt", 2)) == 4);
        expect (static_cast<int> (engine.invoke ("BlueprintNative.squareIt", 3)) == 9);
    }
};

//==============================================================================
class EcmascriptEngineErrorHandlerTest final : public BlueprintUnitTest
{
public:
    EcmascriptEngineErrorHandlerTest() : BlueprintUnitTest ("Error Handler Interfaces") {}

    void runTest() override
    {
        auto noopFunc = [&] (const juce::var::NativeFunctionArgs&)
        {
            return juce::var::undefined();
        };

        auto testAndExpectThrow = [&] (auto&& func)
        {
            bool didThrow = false;

            try
            {
                func();
            }
            catch (...)
            {
                didThrow = true;
            }

            expect (didThrow);
        };

        beginTest ("Parse Errors");

        testAndExpectThrow ([&]() { evaluate ("1 + 2 + "); });
        testAndExpectThrow ([&]() { invoke ("Blueprint.1+", 1); });
        testAndExpectThrow ([&]() { engine.registerNativeMethod ("global[1 + 2 +]", "Noop", noopFunc); });
        testAndExpectThrow ([&]() { engine.registerNativeProperty ("global[1 + 2 +]", "Noop", 42); });

        beginTest ("Runtime Errors");

        testAndExpectThrow ([&]() { evaluate ("doesNotExist();"); });
        testAndExpectThrow ([&]() { invoke ("Blueprint[doesNotExist()]", 1); });
        testAndExpectThrow ([&]() { engine.registerNativeMethod ("global[doesNotExist()]", "Noop", noopFunc); });
        testAndExpectThrow ([&]() { engine.registerNativeProperty ("global[doesNotExist()]", "Noop", 42); });
    }
};

//==============================================================================
struct SourceFileTest : blueprint::BlueprintUnitTest
{
    SourceFileTest (const juce::File& fileToRun,
                    juce::var expectedEndResult = juce::var::undefined()) :
        BlueprintUnitTest ("Script Evaluation", "Source"),
        sourceFile (fileToRun),
        expectedResult (std::move (expectedEndResult))
    {}

    void runTest() override
    {
        beginTest ("Source File: " + sourceFile.getFullPathName());

        const auto r = evaluate (sourceFile);

        if (! expectedResult.isVoid())
            expect (r == expectedResult);
        else
            expect (r == juce::var::undefined());
    }

private:
    const juce::File sourceFile;
    const juce::var expectedResult;
};

//==============================================================================
#if BLUEPRINT_CREATE_GLOBAL_UNIT_TESTS

static EcmascriptEngineEvaluateTest evTest;
static EcmascriptEngineNativeFunctionTest fnTest;
static EcmascriptEngineNativePropertyTest propTest;
static EcmascriptEngineInvokeTest invTest;
static EcmascriptEngineErrorHandlerTest errTest;

#endif

} // blueprint

//==============================================================================

#endif // DOXYGEN
