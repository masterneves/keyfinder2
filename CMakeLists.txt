cmake_minimum_required(VERSION 3.22)

project(KeyScaleDetector VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)            # C++17 também funciona (nada aqui exige C++20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- JUCE -------------------------------------------------------------------
# Opção A: JUCE clonado numa subpasta ./JUCE  (git submodule add https://github.com/juce-framework/JUCE)
add_subdirectory(JUCE)

# Opção B: FetchContent (descomentar e remover a Opção A)
# include(FetchContent)
# FetchContent_Declare(JUCE GIT_REPOSITORY https://github.com/juce-framework/JUCE.git GIT_TAG 8.0.4)
# FetchContent_MakeAvailable(JUCE)

# --- Formatos ---------------------------------------------------------------
set(KSD_FORMATS VST3 Standalone)
if(APPLE)
    list(APPEND KSD_FORMATS AU)
endif()

juce_add_plugin(KeyScaleDetector
    COMPANY_NAME                "YourCompany"
    PLUGIN_MANUFACTURER_CODE    Yrco
    PLUGIN_CODE                 Ksdt
    FORMATS                     ${KSD_FORMATS}
    PRODUCT_NAME                "Key Scale Detector"
    IS_SYNTH                    FALSE
    NEEDS_MIDI_INPUT            FALSE
    NEEDS_MIDI_OUTPUT           FALSE
    IS_MIDI_EFFECT              FALSE
    COPY_PLUGIN_AFTER_BUILD     TRUE
    VST3_CATEGORIES             Fx Analyzer
    AU_MAIN_TYPE                kAudioUnitType_Effect
    # Necessário para a entrada de áudio no modo Standalone (macOS)
    MICROPHONE_PERMISSION_ENABLED TRUE
    MICROPHONE_PERMISSION_TEXT  "O plugin precisa de aceder à entrada de áudio para detetar o tom.")

target_sources(KeyScaleDetector PRIVATE
    Source/ChromaExtractor.cpp
    Source/ChromaExtractor.h
    Source/KeyClassifier.cpp
    Source/KeyClassifier.h
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h)

target_compile_definitions(KeyScaleDetector PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_USE_MP3AUDIOFORMAT=1)       # permite ler .mp3 com o MP3AudioFormat do JUCE

target_link_libraries(KeyScaleDetector
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_dsp
        juce::juce_gui_basics
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
