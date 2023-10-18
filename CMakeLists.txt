cmake_minimum_required(VERSION 3.25)
project(FunTableSynth VERSION 0.0.0.1)
set(CMAKE_CXX_STANDARD 17)

find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

add_subdirectory(${JUCE_DIR} BINARY)

juce_add_plugin(FunTableSynth
        FORMATS AU VST3 Standalone
        PRODUCT_NAME "FUN_SHAPER"
        DESCRIPTION "A wave shaper plugin you can customize with function expressions."
        BUNDLE_ID "me.zxning.audio.fun_shaper"FunTableSynth
        IS_SYNTH FALSE
        IS_MIDI_EFFECT FALSE
        NEEDS_MIDI_INPUT FALSE
        NEEDS_MIDI_OUTPUT FALSE
        PLUGIN_MANUFACTURER_CODE Zxst
        PLUGIN_CODE Fnsp
        )

juce_generate_juce_header(FunTableSynth)

file(GLOB Resource "resources/*")

juce_add_binary_data(JuceBinary SOURCES "src/py/internal.py" "${Resource}")

file(GLOB sources "src/*.h" "src/*.cpp" "src/ui/*.h" "src/ui/*.cpp")

target_sources(FunTableSynth PRIVATE ${sources})

target_compile_definitions(FunTableSynth
        PRIVATE
        JUCER_ENABLE_GPL_MODE=1
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_WEB_BROWSER=0
        #        JUCE_USE_WIN_WEBVIEW2=1
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        DONT_SET_USING_JUCE_NAMESPACE=1
        )

target_link_libraries(FunTableSynth
        PRIVATE
        JuceBinary
        Python3::Python
        juce::juce_audio_basics
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_dsp
        juce::juce_gui_basics
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
        )