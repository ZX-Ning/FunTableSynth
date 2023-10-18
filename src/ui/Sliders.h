#ifndef ZX_FUNTABLE_SYNTH_SLIDERS_H_
#define ZX_FUNTABLE_SYNTH_SLIDERS_H_

#include <JuceHeader.h>

namespace ZX {

struct CommonSliderLabel : public juce::Component {
    juce::Slider slider;
    juce::Label label;
    explicit CommonSliderLabel(const char *label, juce::Slider::SliderStyle);
    void resized() override;
};

struct SliderWithLabel : public CommonSliderLabel {
    explicit SliderWithLabel(const char *label);
};

struct KnobWithLabel : public CommonSliderLabel {
    explicit KnobWithLabel(const char *label);
};

}

#endif //ZX_FUNTABLE_SYNTH_SLIDERS_H_
