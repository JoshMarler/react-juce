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
    searchpaths:        ./duktape/src-noline/
   END_JUCE_MODULE_DECLARATION
*******************************************************************************/

#pragma once

#define BLUEPRINT_H_INCLUDED


#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "yoga/yoga/Utils.h"
#include "yoga/yoga/YGConfig.h"
#include "yoga/yoga/YGEnums.h"
#include "yoga/yoga/YGFloatOptional.h"
#include "yoga/yoga/YGLayout.h"
#include "yoga/yoga/YGMacros.h"
#include "yoga/yoga/YGNode.h"
#include "yoga/yoga/YGNodePrint.h"
#include "yoga/yoga/YGStyle.h"
#include "yoga/yoga/Yoga-internal.h"
#include "yoga/yoga/Yoga.h"

#include "duktape/src-noline/duktape.h"
#include "duktape/extras/console/duk_console.h"

#include "core/blueprint_ImageView.h"
#include "core/blueprint_RawTextView.h"
#include "core/blueprint_ReactApplicationRoot.h"
#include "core/blueprint_ShadowView.h"
#include "core/blueprint_TextShadowView.h"
#include "core/blueprint_TextView.h"
#include "core/blueprint_View.h"
