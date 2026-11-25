# ARIA — the plugin (VST3, AU on the Mac) and the standalone app, with JUCE 8.
#   cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build --config Release
# JUCE comes from GitHub (or -DARIA_JUCE_DIR=/path/to/JUCE).
cmake_minimum_required(VERSION 3.22)

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "")
    if(NOT CMAKE_OSX_ARCHITECTURES)
        set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
    endif()
endif()
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

project(ARIA VERSION 1.3.2 LANGUAGES C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ARIA_JUCE_DIR "" CACHE PATH "a JUCE checkout (empty: from GitHub)")
set(ARIA_FORMATS "" CACHE STRING "the formats (empty: VST3 and Standalone, AU too on the Mac)")
if(ARIA_JUCE_DIR)
    add_subdirectory(${ARIA_JUCE_DIR} juce)
else()
    include(FetchContent)
    FetchContent_Declare(juce
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 8.0.9
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(juce)
endif()

if(ARIA_FORMATS)
    set(FORMATS ${ARIA_FORMATS})
else()
    set(FORMATS VST3 Standalone)
    if(APPLE)
        list(APPEND FORMATS AU)
    endif()
endif()

juce_add_plugin(Aria
    PRODUCT_NAME "ARIA"
    COMPANY_NAME "swtdr34mz"
    BUNDLE_ID "com.swtdr34mz.aria"
    PLUGIN_MANUFACTURER_CODE Swtd
    PLUGIN_CODE Aria
    FORMATS ${FORMATS}
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    VST3_CATEGORIES Instrument Synth
    AU_MAIN_TYPE kAudioUnitType_MusicDevice
    MICROPHONE_PERMISSION_ENABLED FALSE)

juce_add_binary_data(AriaData SOURCES
    fonts/FiraSans-SemiBold.ttf
    fonts/FiraSans-Bold.ttf
    fonts/AnonymousPro-Bold.ttf
    res/swtdr34mz-tag.png)

target_sources(Aria PRIVATE
    src/PluginProcessor.cpp
    src/PluginEditor.cpp
    src/Art.cpp
    dsp/aria.c
    dsp/params.c
    dsp/params_data.c
    dsp/presets.c)

target_include_directories(Aria PRIVATE dsp src)

target_compile_definitions(Aria PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0)

target_link_libraries(Aria
    PRIVATE
        AriaData
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
