/*
  ==============================================================================

    blueprint_EcmascriptEngineTests.cpp
    Created: 26 Oct 2019 3:47:39pm

  ==============================================================================
*/


class EcmascriptEngineEvaluateTest  : public juce::UnitTest
{
public:
    EcmascriptEngineEvaluateTest()
        : juce::UnitTest ("Testing script evaluation") {}

    void runTest() override
    {
        blueprint::EcmascriptEngine engine;

        beginTest ("Numbers");
        expect (5 == (int) engine.evaluate("2 + 3;"));
        expect (6 == (int) engine.evaluate("2 * 3;"));
        expect (4 == (int) engine.evaluate("Math.pow(2, 2);"));

        beginTest ("Strings");
        expect (juce::String("hey") == engine.evaluate("['h', 'e', 'y'].join('');").toString());
    }
};

class EcmascriptEngineNativeFunctionTest  : public juce::UnitTest
{
public:
    EcmascriptEngineNativeFunctionTest()
        : juce::UnitTest ("Testing native function interface") {}

    void runTest() override
    {
        blueprint::EcmascriptEngine engine;

        beginTest ("Global function");

        engine.registerNativeMethod("myMultiply", [](void* stash, const juce::var::NativeFunctionArgs& args) {
            EcmascriptEngineNativeFunctionTest* self = reinterpret_cast<EcmascriptEngineNativeFunctionTest*>(stash);

            jassert (self != nullptr);
            self->expect(args.numArguments == 2);

            int left = args.arguments[0];
            int right = args.arguments[1];

            return juce::var(left * right);
        }, (void *) this);

        expect (6 == (int) engine.evaluate("myMultiply(2, 3);"));
        expect (4 == (int) engine.evaluate("this.myMultiply(2, 2);"));

        beginTest ("Namespaced function");
        expect (engine.execute("this.Blueprint = {};").wasOk());

        engine.registerNativeMethod("Blueprint", "squareIt", [](void* stash, const juce::var::NativeFunctionArgs& args) {
            EcmascriptEngineNativeFunctionTest* self = reinterpret_cast<EcmascriptEngineNativeFunctionTest*>(stash);

            jassert (self != nullptr);
            self->expect(args.numArguments == 1);

            int left = args.arguments[0];

            return juce::var(left * left);
        }, (void *) this);

        expect (4 == (int) engine.evaluate("Blueprint.squareIt(2);"));
        expect (9 == (int) engine.evaluate("this.Blueprint.squareIt(3);"));
    }
};

// Create static instances to register it with the array run by
// UnitTestRunner::runAllTests()
static EcmascriptEngineEvaluateTest evTest;
static EcmascriptEngineNativeFunctionTest fnTest;
