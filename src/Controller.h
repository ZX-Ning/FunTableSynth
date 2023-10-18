#ifndef ZX_FUN_SHAPER_CONTROLLER_H_
#define ZX_FUN_SHAPER_CONTROLLER_H_

#include <JuceHeader.h>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include "ui/MainUI.h"
#include "PyInterpreter.h"
#include "ui/PluginEditor.h"

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
    bool _isChanged{true};   // check has the table changed after last get
    MainUi * getMainUi() const;

public:
    explicit Controller(AudioPluginAudioProcessor *_processor);
    Controller(Controller &) = delete;
    Controller(Controller &&) = delete;
    static const char *defaultCode;
    static const char *defaultErrmsg;
    std::string lastPyCode{defaultCode};
    std::string lastErrMsg{defaultErrmsg};

    bool setPyCode();
    void setTimeOut(double);
    void startCalc(double f, double t, double step, double p);
    void updateTable(std::vector<double> &&);   //move table in
    bool isChanged();
    std::vector<float> getTableCopyFloat(bool setChangedToFalse);
    std::unique_ptr<juce::XmlElement> serializeState();
    void restoreState(juce::XmlElement &);
    APVTS *getApvts();
    
};

}// namespace ZX
#endif//ZX_FUN_SHAPER_CONTROLLER_H_