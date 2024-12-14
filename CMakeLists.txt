cmake_minimum_required(VERSION 3.15)
project(reactjuce VERSION 0.1.0)

juce_add_module(react_juce)

add_subdirectory(react_juce/hermes)

target_link_libraries(
    react_juce
    INTERFACE
    hermesapi
)
