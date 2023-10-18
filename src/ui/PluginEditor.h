#ifndef JUCE_PLUGIN_EDITOR_GEN_
#define JUCE_PLUGIN_EDITOR_GEN_

#include "MainUI.h"
#include <JuceHeader.h>

//==============================================================================
class AudioPluginAudioProcessor;

class AudioPluginAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
    explicit AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &);

    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics &) override;
    void resized() override;
    ZX::MainUi *getMainUi();

private:
    AudioPluginAudioProcessor &processorRef;
    ZX::MainUi mainUi;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};

#endif //JUCE_PLUGIN_EDITOR_GEN_