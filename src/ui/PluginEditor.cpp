#include "../PluginProcessor.h"
#include "MainUI.h"
#include "PluginEditor.h"
#include "../helpers.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &p)
        : AudioProcessorEditor(&p), processorRef(p), mainUi(p.getController()) {
    // juce::LookAndFeel::setDefaultLookAndFeel(&lAndF);
    // setLookAndFeel(&lAndF);
    addAndMakeVisible(mainUi);
    setResizable(true, false);
    setSize(800, 350);
    setResizeLimits(800,340, 1200,560);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() = default;

ZX::MainUi *AudioPluginAudioProcessorEditor::getMainUi(){
    return &mainUi;
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint(juce::Graphics &g) {
    // (Our component is opaque, so we must completely fill the background with a solid colour)
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
    juce::String str;
//    str << "Size: " << getWidth() << " x " << getHeight();
    juce::Logger::outputDebugString(str);
}

