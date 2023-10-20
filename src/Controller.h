#ifndef ZX_FUN_TABLE_SYNTH_CONTROLLER_H_
#define ZX_FUN_TABLE_SYNTH_CONTROLLER_H_

#include "PyInterpreter.h"
#include "ui/MainUI.h"
#include "ui/UiState.h"

#include <JuceHeader.h>
#include <functional>
#include <mutex>
#include <string>


class AudioPluginAudioProcessor;

namespace ZX {

using APVTS = juce::AudioProcessorValueTreeState;

class Controller {
private:
    AudioPluginAudioProcessor *processor;
    APVTS apvts;
    std::vector<double> table;
    std::mutex lock;
    PyInterpreter py;
    double timeOut{2000};
    bool _isChanged{true};// check has the table changed after last get
    MainUi *getMainUi() const;
    void tryUpdateMainUi() const;

public:
    explicit Controller(AudioPluginAudioProcessor *_processor);
    Controller(Controller &) = delete;
    Controller(Controller &&) = delete;
    static const char *defaultCode;
    static const char *defaultErrmsg;
    UiState uiState{defaultCode, defaultErrmsg, true, {}};

    bool setPyCode(const char *);
    void setTimeOut(double);
    void startCalc(double f, double t, double step, double p);
    void setAndStartTable();
    void updateTable(std::vector<double> &&);//move table in
    bool isChanged();
    std::vector<float> getTableCopyFloat(bool setChangedToFalse);
    std::vector<float> tryGetTableCopy();
    std::unique_ptr<juce::XmlElement> serializeState();
    void restoreState(juce::XmlElement &);
    APVTS *getApvts();
};

}// namespace ZX
#endif//ZX_FUN_TABLE_SYNTH_CONTROLLER_H_