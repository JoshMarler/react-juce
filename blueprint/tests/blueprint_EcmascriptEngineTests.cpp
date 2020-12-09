//==============================================================================
class EcmascriptEngineEvaluateTest final : public juce::UnitTest
{
public:
    EcmascriptEngineEvaluateTest()
        : juce::UnitTest ("Testing script evaluation") {}

    void runTest() override
    {
        blueprint::EcmascriptEngine engine;

        beginTest ("Numbers");
        expect (static_cast<int> (engine.evaluate("2 + 3;")) == 5);
        expect (static_cast<int> (engine.evaluate("2 * 3;")) == 6);
        expect (static_cast<int> (engine.evaluate("Math.pow(2, 2);")) == 7);

        beginTest ("Strings");
        expect (engine.evaluate("['h', 'e', 'y'].join('');").toString() == "hey");
    }
};

//==============================================================================
class EcmascriptEngineNativeFunctionTest final : public juce::UnitTest
{
public:
    EcmascriptEngineNativeFunctionTest()
        : juce::UnitTest ("Testing native function interface") {}

    void runTest() override
    {
        blueprint::EcmascriptEngine engine;

        beginTest ("Global function");

        engine.registerNativeMethod ("myMultiply", 
        [&] (const juce::var::NativeFunctionArgs& args)
        {
            expect (args.numArguments == 2);

            return static_cast<int> (args.arguments[0]) * static_cast<int> (args.arguments[1]);
        });

        expect (static_cast<int> (engine.evaluate ("myMultiply(2, 3);")) == 6);
        expect (static_cast<int> (engine.evaluate ("this.myMultiply(2, 2);")) == 4);

        beginTest ("Namespaced function");

        engine.registerNativeProperty("Blueprint", juce::JSON::parse("{}"));

        engine.registerNativeMethod ("Blueprint", "squareIt",
        [&] (const juce::var::NativeFunctionArgs& args)
        {
            expect (args.numArguments == 1);

            return juce::square (static_cast<int> (args.arguments[0]));
        });

        expect (4 == static_cast<int> (engine.evaluate ("Blueprint.squareIt(2);")));
        expect (9 == static_cast<int> (engine.evaluate ("this.Blueprint.squareIt(3);")));
    }
};

//==============================================================================
class EcmascriptEngineNativePropertyTest final : public juce::UnitTest
{
public:
    EcmascriptEngineNativePropertyTest()
        : juce::UnitTest ("Testing native property interface") {}

    void runTest() override
    {
        blueprint::EcmascriptEngine engine;

        beginTest ("Global property");
        engine.registerNativeProperty ("BlueprintNative", juce::JSON::parse ("{}"));
        engine.registerNativeProperty ("DOUBLE_PI", juce::MathConstants<double>::twoPi);
        engine.registerNativeProperty ("PLUGIN_VERSION", "1.3.5");

        expect (static_cast<double> (engine.evaluate ("DOUBLE_PI;")) == juce::MathConstants<double>::twoPi);
        expect (engine.evaluate ("PLUGIN_VERSION").toString() == "1.3.5");
        expect (engine.evaluate ("BlueprintNative").isObject());

        beginTest ("Namespaced property");
        engine.registerNativeProperty ("BlueprintNative", "Constants", juce::JSON::parse("{}"));
        engine.registerNativeProperty ("BlueprintNative.Constants", "PLUGIN_VERSION", "1.3.5");
        engine.registerNativeProperty ("BlueprintNative.Constants", "PLUGIN_NAME", "Temper");

        expect (engine.evaluate ("BlueprintNative.Constants.PLUGIN_VERSION").toString() == "1.3.5");
        expect (engine.evaluate ("BlueprintNative.Constants.PLUGIN_NAME").toString() == "Temper");

        auto constants = engine.evaluate("BlueprintNative.Constants");
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
class EcmascriptEngineInvokeTest final : public juce::UnitTest
{
public:
    EcmascriptEngineInvokeTest()
        : juce::UnitTest ("Testing function invocation interface") {}

    void runTest() override
    {
        blueprint::EcmascriptEngine engine;

        beginTest ("Invoking builtins");
        expect (engine.invoke ("String.fromCharCode", 42).toString() == "*");
        expect (static_cast<int> (engine.invoke("parseInt", 1.00031439, 10)) == 1);
        expect (static_cast<int> (engine.invoke("Math.abs", -1)) == 1);

        beginTest ("Invoking native methods");
        engine.registerNativeProperty ("BlueprintNative", juce::JSON::parse ("{}"));

        engine.registerNativeMethod ("BlueprintNative", "squareIt",
        [&] (const juce::var::NativeFunctionArgs& args)
        {
            expect (args.numArguments == 1);

            return juce::square (static_cast<int> (args.arguments[0]));
        });

        expect (static_cast<int> (engine.invoke ("BlueprintNative.squareIt", 2)) == 4);
        expect (static_cast<int> (engine.invoke ("BlueprintNative.squareIt", 3)) == 9);
    }
};

//==============================================================================
class EcmascriptEngineErrorHandlerTest final : public juce::UnitTest
{
public:
    EcmascriptEngineErrorHandlerTest()
        : juce::UnitTest ("Testing error handler interface") {}

    void runTest() override
    {
        testParseErrors();
        testRuntimeErrors();
    }

private:
    template<typename Callable, typename... Args>
    void testUncaughtError (Callable&& fn, Args... args)
    {
        blueprint::EcmascriptEngine engine;
        bool didThrow = false;

        try
        {
            std::invoke (fn, engine, std::forward<Args> (args)...);
        }
        catch (...)
        {
            didThrow = true;
        }

        expect (didThrow);
    }

    template<typename Callable, typename... Args>
    void testCaughtError (Callable&& fn, Args... args)
    {
        blueprint::EcmascriptEngine engine;
        bool didThrow = false;

        engine.onUncaughtError = [&] (const juce::String& msg, const juce::String& trace)
        {
            juce::ignoreUnused (msg, trace);

            didThrow = true;
        };

        std::invoke (fn, engine, std::forward<Args> (args)...);

        expect (didThrow);
    }

    void testParseErrors()
    {
        beginTest ("Parse errors");

        testUncaughtError (&blueprint::EcmascriptEngine::evaluate, juce::String ("1 + 2 + "));
        //testCaughtError (&blueprint::EcmascriptEngine::evaluate, "1 + 2 + ");

        //testUncaughtError (&blueprint::EcmascriptEngine::invoke<int>, "Blueprint.1+", 1);
        //testCaughtError (&blueprint::EcmascriptEngine::invoke<int>, "Blueprint.1+", 1);

        testUncaughtError ([] (blueprint::EcmascriptEngine& engine)
        {
            engine.registerNativeMethod ("global[1 + 2 +]", "Noop",
            [&] (const juce::var::NativeFunctionArgs&)
            {
                return juce::var::undefined();
            });
        });

        testCaughtError ([] (blueprint::EcmascriptEngine& engine)
        {
            engine.registerNativeMethod ("global[1 + 2 +]", "Noop",
            [&] (const juce::var::NativeFunctionArgs&)
            {
                return juce::var::undefined();
            });
        });

        testUncaughtError ([] (blueprint::EcmascriptEngine& engine)
        {
            engine.registerNativeProperty ("global[1 + 2 +]", "Noop", 42);
        });

        testCaughtError ([] (blueprint::EcmascriptEngine& engine)
        {
            engine.registerNativeProperty ("global[1 + 2 +]", "Noop", 42);
        });
    }

    void testRuntimeErrors()
    {
        beginTest ("Runtime errors");

        //testUncaughtError (&blueprint::EcmascriptEngine::evaluate, "doesNotExist();");
        //testCaughtError (&blueprint::EcmascriptEngine::evaluate, "doesNotExist();");

        //testUncaughtError (&blueprint::EcmascriptEngine::invoke<int>, "Blueprint[doesNotExist()]", 1);
        //testCaughtError (&blueprint::EcmascriptEngine::invoke<int>, "Blueprint[doesNotExist()]", 1);

        testUncaughtError ([] (blueprint::EcmascriptEngine& engine)
        {
            engine.registerNativeMethod ("global[doesNotExist()]", "Noop",
            [&] (const juce::var::NativeFunctionArgs&)
            {
                return juce::var::undefined();
            });
        });

        testCaughtError ([] (blueprint::EcmascriptEngine& engine) 
        {
            engine.registerNativeMethod ("global[doesNotExist()]", "Noop",
            [&] (const juce::var::NativeFunctionArgs&)
            {
                return juce::var::undefined();
            });
        });

        testUncaughtError ([] (blueprint::EcmascriptEngine& engine)
        {
            engine.registerNativeProperty ("global[doesNotExist()]", "Noop", 42);
        });

        testCaughtError ([] (blueprint::EcmascriptEngine& engine)
        {
            engine.registerNativeProperty ("global[doesNotExist()]", "Noop", 42);
        });
    }
};

//==============================================================================
static EcmascriptEngineEvaluateTest evTest;
static EcmascriptEngineNativeFunctionTest fnTest;
static EcmascriptEngineNativePropertyTest propTest;
static EcmascriptEngineInvokeTest invTest;
static EcmascriptEngineErrorHandlerTest errTest;
