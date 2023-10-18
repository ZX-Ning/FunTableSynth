#ifndef ZX_FUN_SHAPER_WAVE_SHAPER_H_
#define ZX_FUN_SHAPER_WAVE_SHAPER_H_

#include <JuceHeader.h>
#include <mutex>
#include <vector>

#include "Controller.h"

namespace ZX {

class WaveShaper {
private:
    static constexpr double FROM = -1.0, TO = 1.0;
    Controller *controller;
    std::vector<float> table;
    juce::dsp::Gain<float> gainAfter;
    juce::dsp::WaveShaper<float> shaper;
    uint32_t channels;
    void updateTable();  //get table from controller(skip if not changed)
    float processSample(float) const;
    float linearInterpolation(double index) const;

public: 
    explicit WaveShaper(Controller *_controller);
    void prepareToPlay(double sampleRate, int samplesPerBlock, uint32_t numChannels);
    void processAudio(juce::AudioBuffer<float>&);
};


}// namespace ZX

#endif