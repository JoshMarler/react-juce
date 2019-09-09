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

// Disable compiler warnings for external source files (duktape & yoga)
#if _MSC_VER
  #pragma warning(push)
  #pragma warning(disable : 4018) // signed/unsigned mismatch
  #pragma warning(disable : 4127) // conditional expression is constant
  #pragma warning(disable : 4505) // unreferenced local function
  #pragma warning(disable : 4611) // object destruction is non-portable
  #pragma warning(disable : 4702) // unreachable code
#endif

#include "duktape/src-noline/duktape.c"
#include "duktape/extras/console/duk_console.c"

#include "blueprint.h"

#include "yoga/yoga/log.cpp"
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
#endif

#include "core/blueprint_ReactApplicationRoot.cpp"
#include "core/blueprint_ShadowView.cpp"
#include "core/blueprint_TextShadowView.cpp"
#include "core/blueprint_View.cpp"
