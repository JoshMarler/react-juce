#include <JuceHeader.h>

int main()
{
    try
    {
        juce::ScopedJuceInitialiser_GUI scopedJuceInitialiser;

        juce::UnitTestRunner tr;
        tr.setPassesAreLogged (true);
        tr.runAllTests();
    }
    catch (...)
    {
        jassertfalse;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
