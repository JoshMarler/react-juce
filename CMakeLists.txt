cmake_minimum_required(VERSION 3.15)
project(reactjuce VERSION 0.1.0)

# Change this option to ON if you want to build the AudioPluginHost for example
# (practical to debug the plugin processor directly from your IDE)
option(JUCE_BUILD_EXTRAS "Build JUCE Extras" OFF)


# Change this option to set the JS Interpreter/Engine you wish to run React-JUCE against.
set(REACTJUCE_JS_LIBRARY DUKTAPE CACHE STRING "The JS Engine to use: either HERMES or DUKTAPE")


add_subdirectory(ext/juce)

# Adding any custom modules you might have:
juce_add_module(react_juce)

# Setup the JS engine/interpreter to use.
if (REACTJUCE_JS_LIBRARY STREQUAL "HERMES")
    add_subdirectory(react_juce/hermes)

    target_compile_definitions(
        react_juce
        INTERFACE
        REACTJUCE_USE_HERMES=1
    )

    #TODO: We should be able to remove this include bloc once the following PR is merged
    #      over at hermes upstream: https://github.com/facebook/hermes/pull/454
    target_include_directories(
        react_juce
        INTERFACE
        react_juce/hermes/API/
        react_juce/hermes/public/
    )

    target_link_libraries(
        react_juce
        INTERFACE
        hermesapi
    )
elseif (REACTJUCE_JS_LIBRARY STREQUAL "QUICKJS")
    #TODO: Add QuickJS cmake settings/includes here.
elseif (REACTJUCE_JS_LIBRARY STREQUAL "DUKTAPE")
    target_compile_definitions(
        react_juce
        INTERFACE
        REACTJUCE_USE_DUKTAPE=1
    )
endif()

# If you want to create new projects, you can init them in the examples folder
# and add them here with the add_subdirectory command
add_subdirectory(examples/GainPlugin)
