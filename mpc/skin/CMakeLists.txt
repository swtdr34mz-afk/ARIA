# ARIA's MPC skin renderer: the VST3's own art (src/Art.cpp, src/PluginEditor.cpp's fonts, colours and buttons), each
# MPC page drawn at the MPC's screen size and every control cut out at every value, with a manifest for make_skin.py.
#   cmake -S mpc/skin -B build-skin -DARIA_JUCE_DIR=/path/to/JUCE && cmake --build build-skin
#   build-skin/aria_skin_render mpc/build/skin-render
#   python3 mpc/skin/make_skin.py mpc/build/skin-render mpc/plugin/skins
#   build-skin/aria_skin_render mpc/build/skin-preview --preview 40    (each page as a picture, with the function keys)
# (Only when the pages change: the result, mpc/plugin/skins/, is kept in the repository.)
cmake_minimum_required(VERSION 3.22)
project(AriaSkinRender VERSION 1.0.0 LANGUAGES C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(ARIA_JUCE_DIR "" CACHE PATH "a JUCE checkout (empty: from GitHub)")
if(ARIA_JUCE_DIR)
    add_subdirectory(${ARIA_JUCE_DIR} juce)
else()
    include(FetchContent)
    FetchContent_Declare(juce GIT_REPOSITORY https://github.com/juce-framework/JUCE.git GIT_TAG 8.0.9 GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(juce)
endif()

set(R ${CMAKE_CURRENT_SOURCE_DIR}/../..)
juce_add_console_app(aria_skin_render PRODUCT_NAME "aria_skin_render")
juce_add_binary_data(AriaSkinData SOURCES
    ${R}/fonts/FiraSans-SemiBold.ttf
    ${R}/fonts/FiraSans-Bold.ttf
    ${R}/fonts/AnonymousPro-Bold.ttf
    ${R}/res/swtdr34mz-tag.png)
target_sources(aria_skin_render PRIVATE
    render_skin.cpp
    ${R}/src/PluginProcessor.cpp
    ${R}/src/PluginEditor.cpp
    ${R}/src/Art.cpp
    ${R}/dsp/aria.c
    ${R}/dsp/params.c
    ${R}/dsp/params_data.c
    ${R}/dsp/presets.c)
target_include_directories(aria_skin_render PRIVATE ${R}/dsp ${R}/src)
target_compile_definitions(aria_skin_render PRIVATE
    JucePlugin_Name="ARIA" JucePlugin_IsSynth=1 JucePlugin_WantsMidiInput=1 JucePlugin_ProducesMidiOutput=0
    JucePlugin_IsMidiEffect=0 JucePlugin_EditorRequiresKeyboardFocus=0
    JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_ALSA=0 JUCE_JACK=0 JUCE_DISPLAY_SPLASH_SCREEN=0 JUCE_REPORT_APP_USAGE=0
    JUCE_USE_XRANDR=0 JUCE_USE_XINERAMA=0 JUCE_USE_XSHM=0 JUCE_USE_XRENDER=0 JUCE_USE_XCURSOR=0
    JUCE_MODAL_LOOPS_PERMITTED=0)
target_link_libraries(aria_skin_render PRIVATE AriaSkinData juce::juce_audio_utils juce::juce_recommended_config_flags)
