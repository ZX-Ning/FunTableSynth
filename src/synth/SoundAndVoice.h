#ifndef ZX_FUN_TABLE_SYNTH_SAV_H_
#define ZX_FUN_TABLE_SYNTH_SAV_H_


#include <JuceHeader.h>
#include <vector>
#include "TableProcessor.h"
#include "../Controller.h"

namespace ZX {
class FTSynthSound : public juce::SynthesiserSound {
private:
    std::vector<float> table;
    TableProcessor processor{&table};
    Controller *controller;
public:
    explicit FTSynthSound(Controller *);
    bool appliesToNote(int midiNoteNumber) override;
    bool appliesToChannel(int midiChannel) override;
    void tryUpdateTable();
    float processTable(float x);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FTSynthSound)
};

class FTSynthVoice : public juce::SynthesiserVoice {
private:
    juce::dsp::Oscillator<float> osc;
    juce::ADSR adsr;
    FTSynthSound *currentSound{nullptr};
    Controller *controller;
public:
    explicit FTSynthVoice(Controller *);
    bool canPlaySound(juce::SynthesiserSound *sound) override;
    void prepare(double sampleRate, uint32_t maximumBlockSize, uint32_t numChannels);
    void startNote(int midiNoteNumber, float velocity,
                   juce::SynthesiserSound *sound,
                   int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FTSynthVoice)
};

}


#endif //ZX_FUN_TABLE_SYNTH_SAV_H_