#ifndef ZX_FUN_SHAPER_UI_MAIN_UI_H_
#define ZX_FUN_SHAPER_UI_MAIN_UI_H_

#include "ShaperGraph.h"
#include <JuceHeader.h>
#include <vector>

namespace ZX {

class Controller;

using APVTS = juce::AudioProcessorValueTreeState;

struct SliderWithLabel : public juce::Component{
    juce::Slider slider;
    juce::Label label;
    explicit SliderWithLabel(const char* label);
    void resized() override;
};

class MainUi final : public juce::Component,
                     juce::Button::Listener,
                     juce::TextEditor::Listener {
private:
    Controller *controller;
    SliderWithLabel gainSlider;
    juce::Label title;
    juce::Label gainLabel;
    juce::TextButton calcButton;
    juce::TextEditor errorMsg;
    juce::TextEditor codeEditor;
    ShaperGraph graphPane;
    void buttonClicked(juce::Button *button) override;
    void textEditorTextChanged(juce::TextEditor &editor) override;
    std::unique_ptr<APVTS::SliderAttachment> gainAttach;
    std::vector<juce::Component*> getComponents();

public:
    explicit MainUi(Controller *);
    ~MainUi() override;
    void paint(juce::Graphics &g) override;
    void resized() override;
    std::string getCodeInside();
    void updateCodeEditor(const char*);
    void updateErrorMsg(const char*);
    void updateGraph(std::vector<float>&&);
    void updateButtonEnable(bool);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainUi)
    void initComponents();
};

}// namespace ZX

#endif//ZX_FUN_SHAPER_UI_MAIN_UI_H_