#ifndef TESTVST_MYSYNTH_H
#define TESTVST_MYSYNTH_H

#include <JuceHeader.h>
#include <cstdint>

struct MySynthSound final : public juce::SynthesiserSound {
    std::vector<double> table;

    bool appliesToNote(int midiNoteNumber) override {
        return true;
    }
    bool appliesToChannel(int midiChannel) override {
        return true;
    }

};

struct MySynthVoice final : public juce::SynthesiserVoice {
    juce::dsp::Oscillator<float> osc;
    juce::ADSR adsr;
    MySynthVoice() {
        osc.initialise([](float x) { return std::sinf(x); });
        adsr.setParameters({0.3f, 2.f, 1.f, 0.3f});
    }

    bool canPlaySound(SynthesiserSound *sound) override {
        return typeid(*sound) == typeid(MySynthSound);
    }

    void prepare(double sampleRate, uint32_t maximumBlockSize, uint32_t numChannels) {
//        setCurrentPlaybackSampleRate(sampleRate);
        osc.prepare({sampleRate, maximumBlockSize, numChannels});
        adsr.setSampleRate(sampleRate);
    }

    void startNote(int midiNoteNumber,
                   float velocity,
                   SynthesiserSound *sound,
                   int currentPitchWheelPosition) override {
        osc.setFrequency(static_cast<float>(
                                 juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber)));
        adsr.noteOn();
    }


    void stopNote(float velocity, bool allowTailOff) override {
        adsr.noteOff();
        if ((!allowTailOff) || (!adsr.isActive())) {
            clearCurrentNote();
        }
    }


    void renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override {
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

    void pitchWheelMoved(int newPitchWheelValue) override {}
    void controllerMoved(int controllerNumber, int newControllerValue) override {}
};

struct MySynth: private juce::Synthesiser{
    static constexpr int polyphonic = 4;
    juce::dsp::Gain<float> mainGain;
    bool isPrepared = false;

    explicit MySynth(): juce::Synthesiser(){
        this->addSound(new MySynthSound);
        for (int i = 0; i < polyphonic; ++i) {
            this->addVoice(new MySynthVoice);
        }
    }

    void prepare(double sampleRate, uint32_t maximumBlockSize, uint32_t numChannels){
        this->setCurrentPlaybackSampleRate(sampleRate);
        for (int i = 0; i < this->getNumVoices(); ++i) {
            auto voice = dynamic_cast<MySynthVoice*>(this->getVoice(i));
            voice->prepare(sampleRate, maximumBlockSize, numChannels);
        }
        mainGain.prepare(juce::dsp::ProcessSpec{sampleRate, maximumBlockSize, numChannels});
        isPrepared = true;
    }

    MySynthSound* getMySound(){
        juce::SynthesiserSound* sound = this->getSound(0);
        return dynamic_cast<MySynthSound*>(sound);
    }

    void renderAudio(AudioBuffer<float>& outputAudio, const MidiBuffer& inputMidi,
                     int startSample, int numSamples){
        this->renderNextBlock(outputAudio, inputMidi, startSample, numSamples);
        juce::dsp::AudioBlock<float> block{outputAudio};
        mainGain.process(juce::dsp::ProcessContextReplacing(block));
    }
};

#endif //TESTVST_MYSYNTH_H
