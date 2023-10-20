#include "MainUI.h"

#include "../Controller.h"
#include "../helpers.h"
#include <algorithm>
#include <vector>

namespace ZX {

using namespace juce;

ADSRKnobs::ADSRKnobs()
        : a("Attack"), d("Decay"),
          s("Sustain"), r("Release") {
    addAndMakeVisible(a);
    addAndMakeVisible(d);
    addAndMakeVisible(s);
    addAndMakeVisible(r);
}

void ADSRKnobs::resized() {
    FlexBox flex;
    flex.flexDirection = FlexBox::Direction::row;
    flex.items = juce::Array{
            FlexItem{a}.withFlex(1),
            FlexItem{d}.withFlex(1),
            FlexItem{s}.withFlex(1),
            FlexItem{r}.withFlex(1)
    };
    flex.performLayout(getLocalBounds());
}

MainUi::MainUi(Controller *_controller)
        : controller(_controller),
          gainSlider("Gain:"),
          graphPane(&controller->uiState.currentTable) {
    attaches.push_back(std::make_unique<APVTS::SliderAttachment>(
            *controller->getApvts(), "GAIN", gainSlider.slider));
    attaches.push_back(std::make_unique<APVTS::SliderAttachment>(
            *controller->getApvts(), "ATTACK", adsrKnobs.a.slider));
    attaches.push_back(std::make_unique<APVTS::SliderAttachment>(
            *controller->getApvts(), "DECAY", adsrKnobs.d.slider));
    attaches.push_back(std::make_unique<APVTS::SliderAttachment>(
            *controller->getApvts(), "SUSTAIN", adsrKnobs.s.slider));
    attaches.push_back(std::make_unique<APVTS::SliderAttachment>(
            *controller->getApvts(), "RELEASE", adsrKnobs.r.slider));
    initComponents();
    for (auto *i: getComponents()) {
        addAndMakeVisible(*i);
    }
}

MainUi::~MainUi() {
    saveToState(controller->uiState);
}

void MainUi::initComponents() {
    this->setWantsKeyboardFocus(true);
    title.setText("FUN TABLE\nSYNTH", dontSendNotification);
    title.setFont(getJBMonoFont(true).withHeight(30));
    title.setJustificationType(Justification::left);

    codeEditor.setWantsKeyboardFocus(true);
    codeEditor.setMultiLine(true, false);
    codeEditor.setReturnKeyStartsNewLine(true);
    codeEditor.setTabKeyUsedAsCharacter(true);
    codeEditor.setFont(getJBMonoFont(false).withHeight(16));
    codeEditor.addListener(this);

    calcButton.setButtonText("APPLY");
    calcButton.addListener(this);

    errorMsg.setReadOnly(true);
    errorMsg.setFont(getJBMonoFont(false).withHeight(13));
    errorMsg.setMultiLine(true, true);
    restoreFromState(controller->uiState);
}

void MainUi::paint(juce::Graphics &g) {
}

void MainUi::resized() {
    Grid grid0;
    grid0.templateColumns = {2_fr, 5_fr, 2_fr, 6_fr};
    grid0.templateRows = {1_fr, 1_fr, 1_fr, 1_fr, 1_fr};
    grid0.setGap(10_px);
    auto btnItem = GridItem{calcButton}
            .withArea(5, 3, 6, 4)
            .withJustifySelf(GridItem::JustifySelf::end)
            .withAlignSelf(GridItem::AlignSelf::end);
    btnItem.maxWidth = 120;
    btnItem.maxHeight = 38;
    grid0.items = juce::Array{
            GridItem{title}.withArea(1, 1, 2, 2),
            GridItem{gainSlider}
                    .withArea(2, 1, 6, 2),
            GridItem{graphPane}.withArea(1, 2, 4, 4),
            btnItem,
            GridItem{codeEditor}.withArea(1, 4, 4, 5),
            GridItem{adsrKnobs}.withArea(4, 2, 6, 3),
            GridItem{errorMsg}.withArea(4, 4, 6, 5)};
    grid0.performLayout(getLocalBounds());
}

void MainUi::buttonClicked(juce::Button *button) {
    if (button == &calcButton) {
       controller->setAndStartTable();
    }
}

void MainUi::saveToState(UiState &state) const {
    state.code = codeEditor.getText().toStdString();
    state.errMsg = errorMsg.getText().toStdString();
    state.isButtonEnable = calcButton.isEnabled();
}

void MainUi::restoreFromState(const ZX::UiState &state) {
    codeEditor.setText(state.code);
    errorMsg.setText(state.errMsg);
    calcButton.setEnabled(state.isButtonEnable);
    graphPane.repaint();
}

void MainUi::textEditorTextChanged(juce::TextEditor &editor) {
    controller->uiState.code = editor.getText().toStdString();
}

std::vector<juce::Component *> MainUi::getComponents() {
    return {&gainSlider, &title, &codeEditor, &calcButton, &errorMsg, &graphPane, &adsrKnobs};
}


}// namespace ZX

