#include "MainUI.h"

#include "../Controller.h"
#include "../Resources.h"
#include <algorithm>
#include <vector>

namespace ZX {

using namespace juce;

SliderWithLabel::SliderWithLabel(const char *_label)
    : slider(Slider::SliderStyle::LinearVertical,
             Slider::TextEntryBoxPosition::TextBoxBelow) {
    label.setText(_label, juce::NotificationType::dontSendNotification);
    label.setJustificationType(Justification::horizontallyCentred);
    label.setBorderSize(BorderSize<int>(1, 1, 1, 1));
    label.setFont(getJBMonoFont(false).withHeight(16));
    addAndMakeVisible(label);
    addAndMakeVisible(slider);
}

void SliderWithLabel::resized() {
    FlexBox flex1;
    flex1.flexDirection = FlexBox::Direction::column;
    flex1.items = {
            FlexItem{label}
                    .withFlex(0)
                    .withMinHeight(10)
                    .withMinWidth(50),
            FlexItem{slider}
                    .withFlex(5)};
    flex1.performLayout(getLocalBounds());
}

MainUi::MainUi(Controller *_controller)
    : controller(_controller),
      gainSlider("Gain:") {
    gainAttach = std::make_unique<APVTS::SliderAttachment>(*controller->getApvts(), "GAIN", gainSlider.slider);
    initComponents();
    for (auto *i: getComponents()) {
        addAndMakeVisible(*i);
    }
}

MainUi::~MainUi(){
    controller->lastPyCode = getCodeInside();
    controller->lastErrMsg = errorMsg.getText().toStdString();
}

void MainUi::initComponents() {
    this->setWantsKeyboardFocus(true);
    title.setText("FUN SHAPER", dontSendNotification);
    title.setFont(getJBMonoFont(true).withHeight(32));
    title.setJustificationType(Justification::centred);

    codeEditor.setWantsKeyboardFocus(true);
    codeEditor.setMultiLine(true, false);
    codeEditor.setReturnKeyStartsNewLine(true);
    codeEditor.setTabKeyUsedAsCharacter(true);
    codeEditor.setFont(getJBMonoFont(false).withHeight(16));
    codeEditor.addListener(this);
    codeEditor.setText(controller->lastPyCode);

    calcButton.setButtonText("APPLY");
    calcButton.addListener(this);

    errorMsg.setReadOnly(true);
    errorMsg.setFont(getJBMonoFont(false).withHeight(13));
    errorMsg.setMultiLine(true, true);
    errorMsg.setText(controller->lastErrMsg, false);

    auto table = controller->getTableCopyFloat(false);
    if (!table.empty()){
        graphPane.setTable(std::move(table));
    }
}

void MainUi::paint(juce::Graphics &g) {
}

void MainUi::resized() {
    Grid grid0;
    grid0.templateColumns = {2_fr, 5_fr, 6_fr};
    grid0.templateRows = {1_fr, 1_fr, 1_fr, 1_fr, 1_fr};
    grid0.setGap(20_px);
    auto btnItem = GridItem{calcButton}
                           .withArea(5, 2, 6, 3)
                           .withJustifySelf(GridItem::JustifySelf::end)
                           .withAlignSelf(GridItem::AlignSelf::end);
    btnItem.maxWidth = 120;
    btnItem.maxHeight = 38;
    grid0.items = juce::Array{
            GridItem{title}.withArea(1, 1, 2, 2),
            GridItem{gainSlider}
                    .withArea(2, 1, 6, 2)
                    .withMargin(GridItem::Margin(20, 0, 0, 0)),
            GridItem{graphPane}.withArea(1, 2, 5, 3),
            btnItem,
            GridItem{codeEditor}.withArea(1, 3, 4, 4),
            GridItem{errorMsg}.withArea(4, 3, 6, 4)};
    grid0.performLayout(getLocalBounds());
}

void MainUi::buttonClicked(juce::Button *button) {
    if (button == &calcButton) {
        if (controller->setPyCode()) {
            controller->startCalc(-1, 1, 0.01, 0);
        };
    }
}

void MainUi::textEditorTextChanged(juce::TextEditor &editor) {
}

std::vector<juce::Component *> MainUi::getComponents() {
    return {&gainSlider, &title, &gainLabel, &codeEditor, &calcButton, &errorMsg, &graphPane};
}

std::string MainUi::getCodeInside(){
    return codeEditor.getText().toStdString();
}

void MainUi::updateCodeEditor(const char *code) {
    codeEditor.setText(code, false);
}

void MainUi::updateErrorMsg(const char *msg) {
    errorMsg.setText(msg, false);
}

void MainUi::updateGraph(std::vector<float> &&table) {
//    ZX::_debug_printResult(table);
    this->graphPane.setTable(std::move(table));
    this->graphPane.repaint();
}

void MainUi::updateButtonEnable(bool enable) {
    calcButton.setEnabled(enable);
}

}// namespace ZX


/*
TopFlex(-):
    grid0:
        row1:
            Text("Fun Shaper")<2>
            gainKnob<2>
        row2:
            Graph
            calcButton
    Flex3(|)<1>:
        Text("Function Code")<1>
        TextEditor #code<6>
        Text #error msg<2>
 */