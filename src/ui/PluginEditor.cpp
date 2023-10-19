#include "../PluginProcessor.h"
#include "MainUI.h"
#include "PluginEditor.h"
#include "../helpers.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &p)
        : AudioProcessorEditor(&p), processorRef(p), mainUi(p.getController()) {
    addAndMakeVisible(mainUi);
    setResizable(true, false);
    setSize(1060, 520);
    setResizeLimits(1050,500,
                    1400,700);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() = default;

ZX::MainUi *AudioPluginAudioProcessorEditor::getMainUi(){
    return &mainUi;
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint(juce::Graphics &g) {
    g.fillAll(ZX::colorFromHex(0x263238));
}

void AudioPluginAudioProcessorEditor::resized() {
    juce::FlexBox flex1;
    flex1.items = {
            juce::FlexItem{mainUi}
                    .withFlex(1)
                    .withMargin(10)
    };
    flex1.performLayout(getLocalBounds());
}

