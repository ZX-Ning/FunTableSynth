#ifndef ZX_FUN_TABLE_SYNTH_UI_MAIN_UI_H_
#define ZX_FUN_TABLE_SYNTH_UI_MAIN_UI_H_

#include "ShaperGraph.h"
#include "UiState.h"
#include "Sliders.h"
#include <JuceHeader.h>
#include <vector>


namespace ZX {

class Controller;

using APVTS = juce::AudioProcessorValueTreeState;

struct ADSRKnobs : public juce::Component {
    KnobWithLabel a;
    KnobWithLabel d;
    KnobWithLabel s;
    KnobWithLabel r;
    explicit ADSRKnobs();
    void resized() override;
};

class MainUi final : public juce::Component,
                     juce::Button::Listener,
                     juce::TextEditor::Listener {
private:
    Controller *controller;
    SliderWithLabel gainSlider;
    juce::Label title;
    juce::TextButton calcButton;
    juce::TextEditor errorMsg;
    juce::TextEditor codeEditor;
    ShaperGraph graphPane;
    ADSRKnobs adsrKnobs;
    void buttonClicked(juce::Button *button) override;
    void textEditorTextChanged(juce::TextEditor &editor) override;
    std::vector<std::unique_ptr<APVTS::SliderAttachment>> attaches;
    std::vector<juce::Component*> getComponents();
    void saveToState(UiState&) const;

public:
    explicit MainUi(Controller *);
    ~MainUi() override;
    void paint(juce::Graphics &g) override;
    void resized() override;
    void initComponents();
    void restoreFromState(const ZX::UiState &state) ;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainUi)
};

}// namespace ZX

#endif//ZX_FUN_TABLE_SYNTH_UI_MAIN_UI_H_