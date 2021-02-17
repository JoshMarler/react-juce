/*******************************************************************************
 * This file declares a JUCE module for the shared library code implemented in
 * this directory. The block below is read by the Projucer to automatically
 * generate project code that uses the module. For details, see the
 * JUCE Module Format.txt file in the JUCE directory.

   BEGIN_JUCE_MODULE_DECLARATION
    ID:                 react_juce
    vendor:             Nick Thompson
    version:            0.1.0
    name:               React-JUCE
    description:        Write cross-platform native apps with React.js and JUCE
    minimumCppStandard: 17
    dependencies:       juce_gui_basics
    searchpaths:        ./ ./duktape/ ./duktape/src-noline/ ./yoga
   END_JUCE_MODULE_DECLARATION
*******************************************************************************/

#pragma once

#define REACTJUCE_H_INCLUDED

#include <juce_gui_basics/juce_gui_basics.h>

#if JUCE_MODULE_AVAILABLE_juce_audio_processors
    #include <juce_audio_processors/juce_audio_processors.h>
#endif

//==============================================================================
#if JUCE_EXCEPTIONS_DISABLED
    #error "React-JUCE module requires exceptions to be enabled!"
#endif

#if ! JUCE_CXX17_IS_AVAILABLE
    #error "React-JUCE module requires a C++17 compatible toolchain!"
#endif

//==============================================================================
#include "core/AppHarness.h"
#include "core/EcmascriptEngine.h"
#include "core/CanvasView.h"

#if JUCE_MODULE_AVAILABLE_juce_audio_processors
    #include "core/GenericEditor.h"
#endif

#include "core/ImageView.h"
#include "core/FileWatcher.h"
#include "core/RawTextView.h"
#include "core/ReactApplicationRoot.h"
#include "core/ScrollView.h"
#include "core/ScrollViewContentShadowView.h"
#include "core/ShadowView.h"
#include "core/TextShadowView.h"
#include "core/TextView.h"
#include "core/TextInputView.h"
#include "core/Utils.h"
#include "core/View.h"
#include "core/ViewManager.h"
