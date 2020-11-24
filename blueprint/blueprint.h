/*******************************************************************************
 * This file declares a JUCE module for the shared library code implemented in
 * this directory. The block below is read by the Projucer to automatically
 * generate project code that uses the module. For details, see the
 * JUCE Module Format.txt file in the JUCE directory.

   BEGIN_JUCE_MODULE_DECLARATION
    ID:                 blueprint
    vendor:             Nick Thompson
    version:            0.1.0
    name:               Blueprint
    description:        A React.js render backend targeting JUCE Components.
    minimumCppStandard: 17
    dependencies:       juce_core, juce_graphics, juce_gui_basics
    searchpaths:        ./duktape/src-noline/ ./yoga
   END_JUCE_MODULE_DECLARATION
*******************************************************************************/

#pragma once

#define BLUEPRINT_H_INCLUDED


#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "yoga/yoga/YGMacros.h"

// This is a hacky workaround for an issue introduced in the YG_ENUM_BEGIN
// macro in YGMacros.h. When compiled on VS2017 we fall to their definition
// of YG_ENUM_BEGIN in YGMacros.h, which runs into something like a wrap-around overflow
// with signed types, and ends up trying to index with a garbage value into
// an array. We overwrite the YG_ENUM_BEGIN definition here with an unsigned
// type and everything seems to work ok.
// See: https://github.com/facebook/yoga/issues/891
#ifndef NS_ENUM
#undef YG_ENUM_BEGIN
#define YG_ENUM_BEGIN(name) enum name: unsigned
#endif

// What the hell is wrong with library authors who don't run CI builds with strict warning checks
// and keep their code clean and tidy!?!? As well as missing their own bugs, it means that users of
// their library then hit a million warnings, so they reduce THEIR warning level to avoid the noise,
// (because they'll be too lazy/busy to fix it, or at least to add pragmas like this), and everyone's
// code just ends up a little bit worse..  [/rant]
#if _MSC_VER
 #pragma warning(push)
#elif __clang__
 #pragma clang diagnostic push
 #pragma clang diagnostic ignored "-Wextra-semi"
 #pragma clang diagnostic ignored "-Wsign-conversion"
 #pragma clang diagnostic ignored "-Wswitch-enum"
 #pragma clang diagnostic ignored "-Wunused-parameter"
 #if __clang_major__ > 10
  #pragma clang diagnostic ignored "-Wc++98-compat-extra-semi"
  #pragma clang diagnostic ignored "-Wimplicit-int-conversion"
 #else
  #pragma clang diagnostic ignored "-Wconversion"
 #endif
#elif __GNUC__
 #pragma GCC diagnostic push
 #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
 #pragma GCC diagnostic ignored "-Wsign-conversion"
 #pragma GCC diagnostic ignored "-Wswitch-enum"
 #pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include "yoga/yoga/log.h"
#include "yoga/yoga/event/event.h"
#include "yoga/yoga/YGConfig.h"
#include "yoga/yoga/YGEnums.h"
#include "yoga/yoga/YGFloatOptional.h"
#include "yoga/yoga/YGLayout.h"
#include "yoga/yoga/YGNodePrint.h"
#include "yoga/yoga/YGStyle.h"
#include "yoga/yoga/YGValue.h"
#include "yoga/yoga/Yoga-internal.h"
#include "yoga/yoga/Yoga.h"

#include "duktape/src-noline/duktape.h"
#include "duktape/extras/console/duk_console.h"
#include "duktape/examples/debug-trans-socket/duk_trans_socket.h"

#if _MSC_VER
#elif __clang__
 #pragma clang diagnostic pop
#elif __GNUC__
 #pragma GCC diagnostic pop
#endif

#include "core/blueprint_AppHarness.h"
#include "core/blueprint_EcmascriptEngine.h"
#include "core/blueprint_CanvasView.h"
#include "core/blueprint_GenericEditor.h"
#include "core/blueprint_ImageView.h"
#include "core/blueprint_FileWatcher.h"
#include "core/blueprint_RawTextView.h"
#include "core/blueprint_ReactApplicationRoot.h"
#include "core/blueprint_ScrollView.h"
#include "core/blueprint_ScrollViewContentShadowView.h"
#include "core/blueprint_ShadowView.h"
#include "core/blueprint_TextShadowView.h"
#include "core/blueprint_TextView.h"
#include "core/blueprint_View.h"
#include "core/blueprint_ViewManager.h"
