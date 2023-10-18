#ifndef JUCE_PLUGIN_PROCESSOR_GEN_
#define JUCE_PLUGIN_PROCESSOR_GEN_

#include "Controller.h"
#include "WaveShaper.h"
#include <JuceHeader.h>


//==============================================================================
class AudioPluginAudioProcessor : public juce::AudioProcessor {
public:
    //==============================================================================
    AudioPluginAudioProcessor();

    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;

    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;

    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

    using AudioProcessor::processBlock;

//==============================================================================
#ifndef FOLEYS_
    juce::AudioProcessorEditor *createEditor() override;

    bool hasEditor() const override;
#endif

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;

    bool producesMidi() const override;

    bool isMidiEffect() const override;

    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;

    int getCurrentProgram() override;

    void setCurrentProgram(int index) override;

    const juce::String getProgramName(int index) override;

    void changeProgramName(int index, const juce::String &newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock &destData) override;

    void setStateInformation(const void *data, int sizeInBytes) override;

    ZX::Controller* getController();

private:
    ZX::Controller controller;
    ZX::WaveShaper waveShaper;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessor)
};

#endif//JUCE_PLUGIN_PROCESSOR_GEN_