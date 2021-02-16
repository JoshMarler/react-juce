cmake_minimum_required(VERSION 3.15)
project(reactjuce VERSION 0.1.0)

# Change this option to ON if you want to build the AudioPluginHost for example
# (practical to debug the plugin processor directly from your IDE)
option(JUCE_BUILD_EXTRAS "Build JUCE Extras" OFF)

add_subdirectory(ext/juce)

# Adding any custom modules you might have:
juce_add_module(react_juce)

# If you want to create new projects, you can init them in the examples folder
# and add them here with the add_subdirectory command
add_subdirectory(examples/GainPlugin)
