cmake_minimum_required(VERSION 3.22)

project(SodaFilter VERSION 2.2.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(COPY_AFTER_BUILD "Copy plugins to system folders after build" ON)

# Static link C++ runtime on Windows (must be set before add_subdirectory)
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Minimum macOS version — 10.13 High Sierra for maximum compatibility
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum macOS version")

# Add JUCE
add_subdirectory(JUCE)

# Plugin formats — platform-specific
if(APPLE)
    set(AAX_SDK_PATH "/Users/soda/Documents/aax-sdk-2-9-0")
    set(FORMATS VST3 AU AAX Standalone)
elseif(WIN32)
    set(FORMATS VST3 Standalone)
else()
    set(FORMATS VST3 Standalone)
endif()

# Source files
set(SOURCE_FILES
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/Parameters/ParameterFactory.cpp
    Source/DSP/SaturationEngine.cpp
    Source/DSP/EffectsChain.cpp
    Source/DSP/FlavorProcessor.cpp
    Source/UI/LookAndFeel/ColorScheme.cpp
    Source/UI/LookAndFeel/SodaLookAndFeel.cpp
    Source/UI/Components/FizzKnob.cpp
    Source/UI/Components/FlavorSelector.cpp
    Source/UI/Components/CarbonatedToggle.cpp
    Source/UI/SodaPanel.cpp
)

# Create the plugin target
juce_add_plugin(SodaFilter
    COMPANY_NAME "Carbinated Audio"
    PLUGIN_MANUFACTURER_CODE Mxbs
    PLUGIN_CODE Soda
    FORMATS ${FORMATS}
    PRODUCT_NAME "Carbonator"

    # Plugin characteristics
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE

    # Copy to plugin folders after build (disable with -DCOPY_AFTER_BUILD=OFF on CI)
    COPY_PLUGIN_AFTER_BUILD ${COPY_AFTER_BUILD}

    # Plugin settings
    VST3_CATEGORIES Fx Filter
    AU_MAIN_TYPE kAudioUnitType_Effect
    AAX_CATEGORY AAX_ePlugInCategory_Effect
)

# Add source files
target_sources(SodaFilter PRIVATE ${SOURCE_FILES})

# Add binary resources (fonts)
juce_add_binary_data(SodaFilterBinaryData
    SOURCES
        Resources/Pacifico-Regular.ttf
        Resources/Lobster-Regular.ttf
)

# Compiler definitions
target_compile_definitions(SodaFilter
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_REPORT_APP_USAGE=0
)

if(APPLE AND AAX_SDK_PATH)
    target_compile_definitions(SodaFilter PUBLIC
        JucePlugin_AAXLibs_path="${AAX_SDK_PATH}/Libs"
    )
endif()

# Link libraries
target_link_libraries(SodaFilter
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        SodaFilterBinaryData
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Include directories
target_include_directories(SodaFilter
    PRIVATE
        Source
)

# Platform-specific settings
if(APPLE)
    # Universal binary for macOS (Intel + Apple Silicon)
    set_target_properties(SodaFilter PROPERTIES
        OSX_ARCHITECTURES "arm64;x86_64"
    )
endif()

# Compiler warnings and platform flags
if(MSVC)
    target_compile_options(SodaFilter PRIVATE /W4)
    # Statically link the C++ runtime so users don't need VC++ Redistributable
    set_property(TARGET SodaFilter PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_property(TARGET SodaFilterBinaryData PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else()
    target_compile_options(SodaFilter PRIVATE -Wall -Wextra -Wpedantic)
endif()
