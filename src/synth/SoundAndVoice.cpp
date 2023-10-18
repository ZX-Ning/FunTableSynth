#include "SoundAndVoice.h"

namespace ZX {

FTSynthSound::FTSynthSound(Controller *_controller)
        : controller(_controller) {}

bool FTSynthSound::appliesToNote(int midiNoteNumber) {
    juce::ignoreUnused(midiNoteNumber);
    return true;
}

bool FTSynthSound::appliesToChannel(int midiChannel) {
    juce::ignoreUnused(midiChannel);
    return true;
}

float FTSynthSound::processTable(float x) {
    return processor.processSample(x);
}

void FTSynthSound::tryUpdateTable() {
    auto &&newTable = controller->tryGetTableCopy();
    if (!newTable.empty()) {
        table = newTable;
    }
}

FTSynthVoice::FTSynthVoice(Controller *_controller)
        : controller(_controller) {
    osc.initialise([this](float x) {
        return this->currentSound->processTable(x);
    });

}

bool FTSynthVoice::canPlaySound(juce::SynthesiserSound *sound) {
    return typeid(*sound) == typeid(FTSynthSound);
}

void FTSynthVoice::prepare(double sampleRate, uint32_t maximumBlockSize, uint32_t numChannels) {
    osc.prepare({sampleRate, maximumBlockSize, numChannels});
    adsr.setSampleRate(sampleRate);
}

void FTSynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound,
                             int currentPitchWheelPosition) {
    juce::ignoreUnused(velocity, currentPitchWheelPosition);
    auto apvts = controller->getApvts();
    float a = *(apvts->getRawParameterValue("ATTACK")) / 1000;
    float d = *(apvts->getRawParameterValue("DECAY")) / 1000;
    float s = *(apvts->getRawParameterValue("SUSTAIN"));
    float r = *(apvts->getRawParameterValue("RELEASE")) / 1000;
    adsr.setParameters({a, d, s, r});
    currentSound = dynamic_cast<FTSynthSound *>(sound);
    currentSound->tryUpdateTable();
    osc.setFrequency(static_cast<float>(
                             juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber)));
    adsr.noteOn();
}

void FTSynthVoice::stopNote(float velocity, bool allowTailOff) {
    juce::ignoreUnused(velocity);
    adsr.noteOff();
    if (!allowTailOff) {
        adsr.reset();
        clearCurrentNote();
    }
}

void FTSynthVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) {
    if (!isVoiceActive()) {
        return;
    }
    if (!adsr.isActive()) {
        clearCurrentNote();
        return;
    }
    juce::AudioBuffer<float> bufferWork{outputBuffer.getNumChannels(),
                                        numSamples};
    bufferWork.clear();
    juce::dsp::AudioBlock<float> blockWork{bufferWork};
    osc.process(juce::dsp::ProcessContextReplacing<float>(blockWork));
    adsr.applyEnvelopeToBuffer(bufferWork, 0, numSamples);
    for (int i = 0; i < outputBuffer.getNumChannels(); ++i) {
        outputBuffer.addFrom(i, startSample,
                             bufferWork, i, 0, numSamples);
    }
}

void FTSynthVoice::pitchWheelMoved(int newPitchWheelValue) {}

void FTSynthVoice::controllerMoved(int controllerNumber, int newControllerValue) {}

}