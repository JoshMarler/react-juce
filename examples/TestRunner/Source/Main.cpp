#include <JuceHeader.h>

//==============================================================================
using namespace juce;
using namespace blueprint;

//==============================================================================
#if BLUEPRINT_CREATE_GLOBAL_UNIT_TESTS
    #error "Blueprint: the unit test runner will crash because it's a console app. You need to disable this module configuration."
#endif

//==============================================================================
void printError (const String& str)
{
    const auto text = "ERROR!! " + str;

    if (Logger::getCurrentLogger() != nullptr)
        Logger::writeToLog (text);
    else
        std::cerr << text << std::endl;
}

String createSeparatorString (int numChars)
{
    return String::repeatedString ("=", numChars);
}

//==============================================================================
/** Half-arsed juce::String to juce::var converter. */
var parseStringToVar (const String& source)
{
    enum class NumberType
    {
        unknown,
        integral,
        floatingPoint
    };

    auto parseNumber = [] (const String& s, double& result)
    {
        try
        {
            result = std::stod (s.toStdString());
        }
        catch (...)
        {
            return NumberType::unknown;
        }

        if (s.startsWith ("0x") || ! s.contains ("."))
            return NumberType::integral;

        return NumberType::floatingPoint;
    };

    auto parseBool = [] (const String& s, bool& result)
    {
        result = false;

        if (s.equalsIgnoreCase ("true"))
        {
            result = true;
            return true;
        }

        return s.equalsIgnoreCase ("false");
    };

    bool b = false;

    if (source.isEmpty())                               return var::undefined(); 
    else if (source.equalsIgnoreCase ("undefined"))     return var::undefined();
    else if (parseBool (source, b))                     return b;

    auto v = 0.0;
    switch (parseNumber (source, v))
    {
        case NumberType::integral:      return static_cast<int64> (v);
        case NumberType::floatingPoint: return v;

        default: break;
    };

    return source;
}

//==============================================================================
struct ConsoleLogger final : Logger
{
    ~ConsoleLogger() { Logger::setCurrentLogger (nullptr); }

    void logMessage (const String& message) override
    {
        std::cout << message << std::endl;

       #if JUCE_WINDOWS
        Logger::outputDebugString (message);
       #endif
    }
};

//==============================================================================
struct ConsoleUnitTestRunner final : UnitTestRunner
{
    ConsoleUnitTestRunner()
    {
    }

    void logMessage (const String& message) override
    {
        Logger::writeToLog (message);
    }

    void addStandardTests()
    {
        standardTests =
        {
            new EcmascriptEngineEvaluateTest(),
            new EcmascriptEngineNativeFunctionTest(),
            new EcmascriptEngineNativePropertyTest(),
            new EcmascriptEngineInvokeTest(),
            new EcmascriptEngineErrorHandlerTest()
        };
    }

    void addSourceFileTest (const File& file, var expectedResult = var::undefined())
    {
        sourceFileTests.add (new SourceFileTest (file, expectedResult));
    }

    String getSummary (int& numPasses, int& numFailures) const
    {
        numPasses = 0;
        numFailures = 0;

        for (int i = 0; i < getNumResults(); ++i)
        {
            if (auto* result = getResult (i))
            {
                numPasses += result->passes;
                numFailures += result->failures;
            }
        }

        return createSeparatorString (30) + newLine
             + newLine + "Summary:" + newLine + newLine
             + "Total Num Tests Ran:\t" + String (numPasses + numFailures) + newLine
             + "Total Num Passes:\t" + String (numPasses) + newLine
             + "Total Num Failures:\t" + String (numFailures) + newLine
             + newLine;
    }

    OwnedArray<BlueprintUnitTest> standardTests;
    OwnedArray<SourceFileTest> sourceFileTests;
};

//==============================================================================
int main (int argc, char** argv)
{
    // NB: This is needed because the JS engine requires event loops!
    //     Without this, the console app would always crash...
    const ScopedJuceInitialiser_GUI initialiser;

    ConsoleLogger logger;
    Logger::setCurrentLogger (&logger);

    ConsoleUnitTestRunner runner;
    runner.setPassesAreLogged (true);

    const auto printableVersionString = [&]
    {
        auto r = String (ProjectInfo::companyName) + " "
               + ProjectInfo::projectName + " "
               + ProjectInfo::versionString;

        return r.trim();
    }();

    ArgumentList args (argc, argv);

    if (args.containsOption ("--help|-h"))
    {
        auto createLine = [&] (const String& command, const String& helpString = {})
        {
            return command + newLine + helpString + newLine;
        };

        auto createOption = [&] (const String& command, const String& helpString)
        {
            return createLine (command, "\t\t\t" + helpString);
        };

        std::cout
            << createLine (printableVersionString,              createSeparatorString (printableVersionString.length()))
            << std::endl
            << createOption ("App Path:",                       args.executableName)
            << std::endl
            << createLine ("Commands:")
            << std::endl
            << createOption ("--help, -h",                      "Prints the help and the list of optional commands.")
            << createOption ("--version, -v",                   String ("Prints the version of the ") + ProjectInfo::projectName + ".")
            << createOption ("--list-categories, -l",           "Lists all of the possible unit tests categories to choose from.")
            << createOption ("--category=[category]",           "Forces execution of a particular category of unit tests.")
            << createOption ("--seed=[seed]",                   "Allows explicitly specifying the random seed to use within the unit tests.")
            << createOption ("--file=[absoluteFilePath]",       "Runs a specific script file. This will exclude default unit tests.")
            << createOption ("--expectedResult=[someResult]",   "When paired with --file, allows specifying the expected result when running the provided script.")
            << std::endl
            << createLine ("Note:",                             "All commands are optional, and not specifying a command will run all unit tests!")
            << std::endl
        << std::endl;

        return EXIT_SUCCESS;
    }
    else if (args.containsOption ("--version|-v"))
    {
        std::cout << printableVersionString << std::endl;

        return EXIT_SUCCESS;
    }
    else if (args.containsOption ("--list-categories|-l"))
    {
        for (const auto& category : UnitTest::getAllCategories())
            std::cout << category << std::endl;

        return EXIT_SUCCESS;
    }

    try
    {
        const auto parsedSeed = [&]
        {
            if (args.containsOption ("--seed"))
            {
                const auto seedValueString = args.getValueForOption ("--seed").trim();

                if (seedValueString.isNotEmpty())
                {
                    if (seedValueString.startsWith ("0x"))
                        return seedValueString.getHexValue64();

                    return seedValueString.getLargeIntValue();
                }
            }

            return (int64) 0; //Let the defaults decide.
        }();

        if (args.containsOption ("--file"))
        {
            const auto script = args.getValueForOption ("--file")
                                    .trim()
                                    .unquoted()
                                    .replace ("\\", "/"); // NB: This is for Windows path support.

            if (script.isEmpty())
            {
                printError ("No script file specified with 'run' option!");
                return EXIT_FAILURE;
            }

            if (! File::isAbsolutePath (script))
            {
                printError ("Provided path for script is invalid:\n\t" + script);
                return EXIT_FAILURE;
            }

            const auto s = File (script);
            if (! s.existsAsFile())
            {
                printError ("Provided path does not point to a valid script file:\n\t" + script);
                return EXIT_FAILURE;
            }

            auto expectedResult = var::undefined();
            if (args.containsOption ("--expectedResult"))
                expectedResult = parseStringToVar (args.getValueForOption ("--expectedResult").trim().unquoted());

            runner.addSourceFileTest (s, expectedResult);
            runner.runAllTests (parsedSeed);
        }
        else if (args.containsOption ("--category"))
        {
            runner.addStandardTests();
            runner.runTestsInCategory (args.getValueForOption ("--category").unquoted().trim(), parsedSeed);
        }
        else
        {
            runner.addStandardTests();
            runner.runAllTests (parsedSeed);
        }
    }
    catch (...)
    {
        printError ("We somehow managed to miss catching an exception...");
        return EXIT_FAILURE;
    }

    if (args.containsOption ("--category") && runner.getNumResults() == 0)
    {
        printError ("No unit tests found with the specified category!");
        return EXIT_FAILURE;
    }

    int numPasses = 0, numFailures = 0;
    std::cout << std::endl << runner.getSummary (numPasses, numFailures);
    return numFailures > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
