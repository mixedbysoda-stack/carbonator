cmake_minimum_required(VERSION 3.22)

project(SodaFilter VERSION 2.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# AAX SDK Path
set(AAX_SDK_PATH "/Users/soda/Documents/aax-sdk-2-9-0")

# Add JUCE
add_subdirectory(JUCE)

# Plugin formats to build
set(FORMATS VST3 AU AAX Standalone)

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
    COMPANY_NAME "MixedBySoda"
    PLUGIN_MANUFACTURER_CODE Mxbs
    PLUGIN_CODE Soda
    FORMATS ${FORMATS}
    PRODUCT_NAME "carbonator"

    # Plugin characteristics
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE

    # Copy to plugin folders
    COPY_PLUGIN_AFTER_BUILD TRUE

    # Plugin settings
    VST3_CATEGORIES Fx Filter
    AU_MAIN_TYPE kAudioUnitType_Effect
    AAX_CATEGORY AAX_ePlugInCategory_Effect
)

# Add source files
target_sources(SodaFilter PRIVATE ${SOURCE_FILES})

# Compiler definitions
target_compile_definitions(SodaFilter
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_REPORT_APP_USAGE=0
        JucePlugin_AAXLibs_path="${AAX_SDK_PATH}/Libs"
)

# Link libraries
target_link_libraries(SodaFilter
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
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

# Compiler warnings
if(MSVC)
    target_compile_options(SodaFilter PRIVATE /W4)
else()
    target_compile_options(SodaFilter PRIVATE -Wall -Wextra -Wpedantic)
endif()
