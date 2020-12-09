// What the hell is wrong with library authors who don't run CI builds with strict warning checks
// and keep their code clean and tidy!?!? As well as missing their own bugs, it means that users of
// their library then hit a million warnings, so they reduce THEIR warning level to avoid the noise,
// (because they'll be too lazy/busy to fix it, or at least to add pragmas like this), and everyone's
// code just ends up a little bit worse..  [/rant]
JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wall" "-Wextra-semi" "-Wsign-conversion" "-Wswitch-enum" "-Wshorten-64-to-32"
                                     "-Wzero-as-null-pointer-constant" "-Wunused-parameter" "-Wconversion" "-Wunused-variable"
                                     "-Wredundant-decls")

#if JUCE_MSVC
    #pragma warning (push)
#endif

JUCE_IGNORE_MSVC (4018 4100 4127 4244 4459 4505 4611 4702)

#if __clang_major__ > 10
    #pragma clang diagnostic ignored "-Wc++98-compat-extra-semi"
    #pragma clang diagnostic ignored "-Wimplicit-int-conversion"
    #pragma clang diagnostic ignored "-Wimplicit-float-conversion"
#endif
