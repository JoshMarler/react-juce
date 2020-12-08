#ifdef BLUEPRINT_H_INCLUDED
    /** When you add this cpp file to your project, you mustn't include it in a file where you've
        already included any other headers - just put it inside a file on its own, possibly with your config
        flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
        header files that the compiler may be using.
    */
    #error "Incorrect use of the Blueprint cpp file!"
#endif

//==============================================================================
#define JUCE_CORE_INCLUDE_NATIVE_HEADERS 1

#undef DUK_COMPILING_DUKTAPE
#define DUK_COMPILING_DUKTAPE 1

//==============================================================================
#include "blueprint.h"

//==============================================================================
#include "internal/BeginIgnoringThirdPartyWarnings.h"

extern "C"
{
    #undef DUK_COMPILING_DUKTAPE
    #include "duktape/src-noline/duktape.c"
    #include "duktape/extras/console/duk_console.c"

   #if JUCE_WINDOWS
    #include "duktape/examples/debug-trans-socket/duk_trans_socket_windows.c"
   #else
    #include "duktape/examples/debug-trans-socket/duk_trans_socket_unix.c"
   #endif
}

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

#include "internal/EndIgnoringThirdPartyWarnings.h"

//==============================================================================
#include "core/blueprint_AppHarness.cpp"
#include "core/blueprint_EcmascriptEngine.cpp"
#include "core/blueprint_ReactApplicationRoot.cpp"
#include "core/blueprint_ShadowView.cpp"
#include "core/blueprint_TextShadowView.cpp"
#include "core/blueprint_View.cpp"
#include "core/blueprint_ViewManager.cpp"
#include "core/blueprint_ScrollView.cpp"

#if JUCE_MODULE_AVAILABLE_juce_audio_processors
    #include "core/blueprint_GenericEditor.cpp"
#endif

#if BLUEPRINT_INCLUDE_UNIT_TESTS
    #include "tests/blueprint_EcmascriptEngineTests.cpp"
#endif
