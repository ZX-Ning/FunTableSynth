#include "WaveShaper.h"
#include <cmath>
#include <limits>
#include "helpers.h"

namespace ZX {

WaveShaper::WaveShaper(Controller *_controller)
        : controller(_controller) {}

void WaveShaper::updateTable() {
    if (controller->isChanged()) {
        auto &&newTable = controller->getTableCopyFloat(true);
        table = std::move(newTable);
    }
}

float WaveShaper::processSample(float x) const {
    //return 1 if sample is larger than 1
    x = inRange(x, -1.0f, 1.0f);
    if (table.empty()) {
        return x;
    }
    size_t tableSize = table.size();
    //size = floor((to - fr) / step) + 1
    double step = std::floor(TO - FROM) / (static_cast<double>(tableSize - 1));
    double index = (x - FROM) / step;
    float result = linearInterpolation(index);
    return inRange(result, -1.0f, 1.0f);
}


float WaveShaper::linearInterpolation(double index) const {
    // point in the left: (x1, y1); point in the right; (x2,y2); the result point (index, result)
    size_t x1 = static_cast<size_t>(std::floor(index));
    size_t x2 = static_cast<size_t>(std::ceil(index));
    // prevent out of range
    if (x2 >= table.size() || x1 >= table.size()) {
        return table.back();
    }
    double y1 = table[x1];
    double y2 = table[x2];
    if (x2 == x1) {
        return static_cast<float>((y1 + y2) / 2);
    }
    double slope = (y2 - y1) / (x2 - x1);
    return static_cast<float>(slope * (index - x1) + y1);
}

void WaveShaper::prepareToPlay(double _sampleRate, int _samplesPerBlock, uint32_t numChannels) {
    channels = numChannels;
    gainAfter.prepare({_sampleRate,
                       static_cast<uint32_t>(_samplesPerBlock),
                       numChannels});
}


void WaveShaper::processAudio(juce::AudioBuffer<float> &buffer) {
    updateTable();
    int samples = buffer.getNumSamples();
    for (uint32_t i = 0; i < channels; ++i) {
        float *bufferPtr = buffer.getWritePointer(i);
        for (int j = 0; j < samples; ++j) {
            bufferPtr[j] = processSample(bufferPtr[j]);
        }
    }
    juce::dsp::AudioBlock<float> block(buffer);
    float gain = *controller->getApvts()->getRawParameterValue("GAIN");
    gainAfter.setGainDecibels(gain);
    gainAfter.process(juce::dsp::ProcessContextReplacing{block});
}

}// namespace ZX
