#pragma once

// Disable compiler warnings for yoga
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
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

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

// Enable compiler warnings
#if _MSC_VER
#pragma warning (pop)
#elif __clang__
#pragma clang diagnostic pop
#elif __GNUC__
#pragma GCC diagnostic pop
#endif
