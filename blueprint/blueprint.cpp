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
 * the loading of the duktape header.
 */
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

#include "core/blueprint_ReactApplicationRoot.cpp"
#include "core/blueprint_ShadowView.cpp"
#include "core/blueprint_TextShadowView.cpp"
#include "core/blueprint_View.cpp"
