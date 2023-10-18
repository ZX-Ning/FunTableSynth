#include "Sliders.h"
#include "../helpers.h"

namespace ZX {
using namespace juce;

CommonSliderLabel::CommonSliderLabel(const char *_label, Slider::SliderStyle style)
        : slider(style,
                 Slider::TextEntryBoxPosition::TextBoxBelow) {
    label.setText(_label, juce::NotificationType::dontSendNotification);
    label.setJustificationType(Justification::horizontallyCentred);
    label.setBorderSize(BorderSize<int>(1, 1, 1, 1));
    label.setFont(getJBMonoFont(false).withHeight(16));
    label.setJustificationType(Justification::Flags::centred);
    addAndMakeVisible(label);
    addAndMakeVisible(slider);
}

void CommonSliderLabel::resized() {
    FlexBox flex1;
    flex1.flexDirection = FlexBox::Direction::column;
    flex1.items = {
            FlexItem{label}
                    .withFlex(0)
                    .withMinHeight(15)
                    .withMinWidth(50),
            FlexItem{slider}
                    .withFlex(5)};
    flex1.performLayout(getLocalBounds());
}

SliderWithLabel::SliderWithLabel(const char *_label)
        : CommonSliderLabel(_label, Slider::SliderStyle::LinearVertical) {}

KnobWithLabel::KnobWithLabel(const char *_label)
        : CommonSliderLabel(_label, Slider::SliderStyle::RotaryHorizontalVerticalDrag) {}

}