#include "PluginProcessor.h"

#include "Controller.h"
#include "WaveShaper.h"
#include "ui/PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
        : juce::AudioProcessor(
        BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
), controller(this), waveShaper(&controller) {

}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor() = default;

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const {
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool AudioPluginAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int AudioPluginAudioProcessor::getNumPrograms() {
    return 1;
}

int AudioPluginAudioProcessor::getCurrentProgram() { return 0; }

void AudioPluginAudioProcessor::setCurrentProgram(int index) {
    juce::ignoreUnused(index);
}

const juce::String AudioPluginAudioProcessor::getProgramName(int index) {
    juce::ignoreUnused(index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName(int index,
                                                  const juce::String &newName) {
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay(double sampleRate,
                                              int samplesPerBlock) {
    waveShaper.prepareToPlay(
            sampleRate,
            samplesPerBlock,
            static_cast<uint32_t>(getTotalNumInputChannels()));
}

void AudioPluginAudioProcessor::releaseResources() {
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported(
        const BusesLayout &layouts) const {
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}

void AudioPluginAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                             juce::MidiBuffer &midiMessages) {
    juce::ignoreUnused(midiMessages);                                           
    waveShaper.processAudio(buffer);
}

//==============================================================================

bool AudioPluginAudioProcessor::hasEditor() const {
    return true;
}

juce::AudioProcessorEditor *AudioPluginAudioProcessor::createEditor() {
    return new AudioPluginAudioProcessorEditor(*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation(juce::MemoryBlock &destData) {
    auto xml = controller.serializeState();
    copyXmlToBinary(*xml, destData);
}

void AudioPluginAudioProcessor::setStateInformation(const void *data,
                                                    int sizeInBytes) {
    auto xml = getXmlFromBinary(data, sizeInBytes);
    if (xml != nullptr) {
        controller.restoreState(*xml);
    }
}

ZX::Controller *AudioPluginAudioProcessor::getController() { return &controller; }


juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
    return new AudioPluginAudioProcessor();
}