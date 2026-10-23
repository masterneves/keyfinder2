cmake_minimum_required(VERSION 3.15)
project(KeyScaleDetector VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.0
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(KeyScaleDetector
    COMPANY_NAME "MasterNeves"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Mnev
    PLUGIN_CODE Ksd1
    FORMATS VST3
    JUCE_VST3_CAN_REPLACE_VST2 OFF
)

target_sources(KeyScaleDetector
    PRIVATE
        PluginProcessor.cpp
        PluginProcessor.h
        PluginEditor.cpp
        PluginEditor.h
        ChromaExtractor.cpp
        ChromaExtractor.h
        KeyClassifier.cpp
        KeyClassifier.h
)

target_compile_definitions(KeyScaleDetector
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(KeyScaleDetector
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_gui_extra
        juce::juce_gui_basics
        juce::juce_graphics
        juce::juce_events
        juce::juce_core
        juce::juce_data_structures
)

juce_generate_juce_header(KeyScaleDetector)
