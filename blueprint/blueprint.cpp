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

/* We're careful to include the duktape source files before the module header
 * file because `duktape.c` sets certain preprocessor definitions that enable
 * necessary features in the duktape header. We need those defines to preempt
 * the loading of the duktape header. This also, therefore, is the place for
 * custom preprocessor definitions.
 *
 * We force Duktape to use a time provider on Windows that is compatible with
 * Windows 7 SP1. It looks like W7SP1 is quite happy with plugins built with
 * the 8.1 SDK, but the GetSystemTimePreciseAsFileTime() call used in here is
 * just not supported without the 8.1 dll available.
 */
#if defined (_WIN32) || defined (_WIN64)
#define DUK_USE_DATE_NOW_WINDOWS 1
#endif

/*
 * For whatever reason it is necessary to define this to resolve errors caused by both
 * duktape and juce including parts of the winsock2 API. There may be a better way to
 * resolve this.
 */
#if defined (_WIN32) || defined (_WIN64)
#define _WINSOCKAPI_
#endif

// Disable compiler warnings for external source files (duktape & yoga)
#if _MSC_VER
  #pragma warning(push)
  #pragma warning(disable : 4018) // signed/unsigned mismatch
  #pragma warning(disable : 4127) // conditional expression is constant
  #pragma warning(disable : 4505) // unreferenced local function
  #pragma warning(disable : 4611) // object destruction is non-portable
  #pragma warning(disable : 4702) // unreachable code
#endif


// We rely on the JUCE_DEBUG macro in duk_config.h at the moment to determine
// when we enable duktape debug features. This is a bit of a hack to make this
// work. We should be able to do better and may do so once we enable custom duktape
// configs.
#include <juce_core/system/juce_TargetPlatform.h>

#if __clang__
 #pragma clang diagnostic push
 #pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
 #pragma clang diagnostic ignored "-Wc++98-compat-extra-semi"
 #pragma clang diagnostic ignored "-Wswitch-enum"
 #pragma clang diagnostic ignored "-Wshorten-64-to-32"
 #pragma clang diagnostic ignored "-Wimplicit-int-conversion"
 #pragma clang diagnostic ignored "-Wshadow-field-in-constructor"
 #pragma clang diagnostic ignored "-Wsign-conversion"
 #pragma clang diagnostic ignored "-Wimplicit-float-conversion"
#endif

#include "duktape/src-noline/duktape.c"
#include "duktape/extras/console/duk_console.c"

#if defined (_WIN32) || defined (_WIN64)
    #include "duktape/examples/debug-trans-socket/duk_trans_socket_windows.c"
#else
    #include "duktape/examples/debug-trans-socket/duk_trans_socket_unix.c"
#endif

#include "blueprint.h"

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

#if __clang__
 #pragma clang diagnostic pop
#endif

// Enable compiler warnings
#if _MSC_VER
 #pragma warning (pop)
#endif

#include "core/blueprint_EcmascriptEngine.cpp"
#include "core/blueprint_GenericEditor.cpp"
#include "core/blueprint_ReactApplicationRoot.cpp"
#include "core/blueprint_ShadowView.cpp"
#include "core/blueprint_TextShadowView.cpp"
#include "core/blueprint_View.cpp"
#include "core/blueprint_ViewManager.cpp"

#ifdef BLUEPRINT_INCLUDE_TESTS
#include "tests/blueprint_EcmascriptEngineTests.cpp"
#endif
