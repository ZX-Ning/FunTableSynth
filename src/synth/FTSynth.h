#ifndef ZX_FUN_TABLE_SYNTH_SYNTH_H_
#define ZX_FUN_TABLE_SYNTH_SYNTH_H_

#include <JuceHeader.h>
#include "SoundAndVoice.h"
#include "../Controller.h"

namespace ZX {

class FTSynth : private juce::Synthesiser {
private:
    static constexpr int polyphonic = 8;
    juce::dsp::Gain<float> mainGain;
    bool isPrepared = false;
    Controller *controller;

public:
    explicit FTSynth(Controller *);
    void prepare(double sampleRate, uint32_t maximumBlockSize, uint32_t numChannels);
    FTSynthSound *getMySound();
    void renderAudio(juce::AudioBuffer<float> &outputAudio, const juce::MidiBuffer &inputMidi,
                     int startSample, int numSamples);
};

}


#endif //ZX_FUN_TABLE_SYNTH_SYNTH_H_