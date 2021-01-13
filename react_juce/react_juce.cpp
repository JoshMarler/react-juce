/*
  ==============================================================================

    blueprint.cpp
    Created: 26 Nov 2018 3:19:03pm

  ==============================================================================
*/

#ifdef BLUEPRINT_H_INCLUDED
 /* When you add this cpp file to your project, you mustn't include it in a file where you've
    already included any other headers - just put it inside a file on its own, possibly with your config
    flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
    header files that the compiler may be using.
 */
 #error "Incorrect use of the Blueprint cpp file"
#endif

// Disable compiler warnings for external source files (yoga)
#if _MSC_VER
  #pragma warning(push)
  #pragma warning(disable : 4018) // signed/unsigned mismatch
  #pragma warning(disable : 4127) // conditional expression is constant
  #pragma warning(disable : 4244) // possible loss of data
  #pragma warning(disable : 4505) // unreferenced local function
  #pragma warning(disable : 4611) // object destruction is non-portable
  #pragma warning(disable : 4702) // unreachable code
  #pragma warning(disable : 4100 4244 4459)
#elif __clang__
 #pragma clang diagnostic push
 #pragma clang diagnostic ignored "-Wextra-semi"
 #pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
 #pragma clang diagnostic ignored "-Wswitch-enum"
 #pragma clang diagnostic ignored "-Wshorten-64-to-32"
 #pragma clang diagnostic ignored "-Wshadow-field-in-constructor"
 #pragma clang diagnostic ignored "-Wsign-conversion"
 #pragma clang diagnostic ignored "-Wformat-nonliteral"
 #if __clang_major__ > 10
  #pragma clang diagnostic ignored "-Wc++98-compat-extra-semi"
  #pragma clang diagnostic ignored "-Wimplicit-int-conversion"
  #pragma clang diagnostic ignored "-Wimplicit-float-conversion"
 #else
  #pragma clang diagnostic ignored "-Wconversion"
 #endif
#elif __GNUC__
 #pragma GCC diagnostic push
 #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
 #pragma GCC diagnostic ignored "-Wsign-conversion"
 #pragma GCC diagnostic ignored "-Wswitch-enum"
 #pragma GCC diagnostic ignored "-Wunused-variable"
 #pragma GCC diagnostic ignored "-Wredundant-decls"
#endif

#include "react_juce.h"

#include "yoga/yoga/log.cpp"
#include "yoga/yoga/event/event.cpp"
#include "yoga/yoga/Utils.cpp"
#include "yoga/yoga/YGConfig.cpp"
#include "yoga/yoga/YGEnums.cpp"
#include "yoga/yoga/YGLayout.cpp"
#include "yoga/yoga/YGNode.cpp"
#include "yoga/yoga/YGNodePrint.cpp"
#include "yoga/yoga/YGStyle.cpp"
#include "yoga/yoga/YGValue.cpp"
#include "yoga/yoga/Yoga.cpp"

// Enable compiler warnings
#if _MSC_VER
 #pragma warning (pop)
#elif __clang__
 #pragma clang diagnostic pop
#elif __GNUC__
 #pragma GCC diagnostic pop
#endif

#include "core/AppHarness.cpp"
#include "core/EcmascriptEngine.cpp"

#if JUCE_MODULE_AVAILABLE_juce_audio_processors
    #include "core/GenericEditor.cpp"
#endif

#include "core/ReactApplicationRoot.cpp"
#include "core/ShadowView.cpp"
#include "core/TextShadowView.cpp"
#include "core/View.cpp"
#include "core/ViewManager.cpp"
#include "core/ScrollView.cpp"
#include "core/ImageView.cpp"