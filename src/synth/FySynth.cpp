#include "FTSynth.h"

namespace ZX {

FTSynth::FTSynth(Controller *_controller)
        : juce::Synthesiser(), controller(_controller) {
    this->addSound(new FTSynthSound(controller));
    for (int i = 0; i < polyphonic; ++i) {
        this->addVoice(new FTSynthVoice(controller));
    }
}

void FTSynth::prepare(double sampleRate, uint32_t maximumBlockSize, uint32_t numChannels) {
    this->setCurrentPlaybackSampleRate(sampleRate);
    for (int i = 0; i < this->getNumVoices(); ++i) {
        auto voice = dynamic_cast<FTSynthVoice *>(this->getVoice(i));
        voice->prepare(sampleRate, maximumBlockSize, numChannels);
    }
    mainGain.prepare(juce::dsp::ProcessSpec{sampleRate, maximumBlockSize, numChannels});
    isPrepared = true;
}

FTSynthSound *FTSynth::getMySound() {
    juce::SynthesiserSound *sound = this->getSound(0);
    return dynamic_cast<FTSynthSound *>(sound);
}

void FTSynth::renderAudio(juce::AudioBuffer<float> &outputAudio, const juce::MidiBuffer &inputMidi, int startSample,
                          int numSamples) {
    this->renderNextBlock(outputAudio, inputMidi, startSample, numSamples);
    juce::dsp::AudioBlock<float> block{outputAudio};
    float gain = *(controller->getApvts()->getRawParameterValue("GAIN"));
    mainGain.setGainDecibels(gain);
    mainGain.process(juce::dsp::ProcessContextReplacing(block));
}

}