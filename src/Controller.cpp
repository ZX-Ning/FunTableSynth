#include "Controller.h"
#include "AsyncThread.h"
#include "PluginProcessor.h"
#include "PyInterpreter.h"
#include "ui/PluginEditor.h"

#include <memory>
#include <sstream>
#include <string>
#include <variant>
#include <vector>



namespace ZX {
const char *Controller::defaultCode = "#Enter expression here using python syntax\n"
                                      "#Your code in here will be warpped in a python function\n"
                                      "#and the parameter is x(already imported * from math)\n\n"
                                      "return sin(x)";

const char *Controller::defaultErrmsg = "Welcome to FUN TABLE SYNTH!\n"
                                        "Enter expression above using python syntax and hit apply button to perform calculation.\n"
                                        "Notice: The calculation task will be automatically stopped if it runs so long"
                                        "(current time limit is set to 5 second)";

const int TIME_OUT_MS = 5000;

using MsgMan = juce::MessageManager;
using ListResult = PyInterpreter::ListResult;

static APVTS::ParameterLayout genParameters() {
    return APVTS::ParameterLayout{
            std::make_unique<juce::AudioParameterFloat>(
                    juce::ParameterID("GAIN", 1),
                    "Gain", -48.0f, 0.0f, -6.0f),
            std::make_unique<juce::AudioParameterBool>(
                    juce::ParameterID("DC_OFFSET_REMOVE", 1),
                    "remove DC offset", false),
            std::make_unique<juce::AudioParameterBool>(
                    juce::ParameterID("USING_LOOKUP", 1),
                    "using lookup table", true),
            std::make_unique<juce::AudioParameterFloat>(
                    juce::ParameterID("ATTACK", 1),
                    "Attack", 0.0f, 1500.0f, 0.5f),
            std::make_unique<juce::AudioParameterFloat>(
                    juce::ParameterID("DECAY", 1),
                    "Decay", 0.0f, 3000.0f, 0.0f),
            std::make_unique<juce::AudioParameterFloat>(
                    juce::ParameterID("SUSTAIN", 1),
                    "Sustain", 0.0f, 1.0f, 1.0f),
            std::make_unique<juce::AudioParameterFloat>(
                    juce::ParameterID("RELEASE", 1),
                    "Release", 0.0f, 1000.0f, 0.5f)
    };
}

MainUi *Controller::getMainUi() const {
    auto *editor = processor->getActiveEditor();
    if (editor == nullptr) {
        return nullptr;
    }
    auto *pEditor = dynamic_cast<AudioPluginAudioProcessorEditor *>(editor);
    return pEditor->getMainUi();
}

void Controller::tryUpdateMainUi() const {
    auto ui = getMainUi();
    if (ui != nullptr){
        ui->restoreFromState(uiState);
    }
}

Controller::Controller(AudioPluginAudioProcessor *_processor)
        : processor(_processor), apvts(*_processor, nullptr,
                                       "PARAMETERS", genParameters()) {
}

bool Controller::setPyCode() {
    try {
        std::stringstream iss{uiState.code};
        std::stringstream oss;
        oss << "def osc(x,p):\n";
        while (iss.good()) {
            char buf[100];
            iss.getline(buf, 100);
            oss << "   " << buf << "\n";
        }
        oss << std::endl;
        py.addFunction(oss.str().c_str());
        return true;
    }
    catch (PyException &e) {
        uiState.errMsg = e.what();
        tryUpdateMainUi();
        return false;
    }
}

void Controller::setTimeOut(double _timeOut) { timeOut = _timeOut; }

void Controller::startCalc(double f, double t, double step, double p) {
    if (py.isRunning()) {
        uiState.errMsg = "Error: already running!";
        tryUpdateMainUi();
        return;
    }
    uiState.isButtonEnable = false;
    tryUpdateMainUi();
    auto asyncRunner = AsyncThread::getAsyncRunner();
    uiState.errMsg = "Calculating...";
    tryUpdateMainUi();
    asyncRunner->asyncRun([f, t, step, p, this] {
        std::function<void()> callBack;
        try {
            this->py.startCalc(f, t, step, p);
            auto r = this->py.waitAndGet(TIME_OUT_MS);
            if (auto *str = std::get_if<std::string>(&r)) {
                std::string &errmsg = *str;
                callBack = [this, errmsg = std::move(errmsg)] {
                    uiState.errMsg = errmsg;
                    tryUpdateMainUi();
                };
            }
            else if (auto *list = std::get_if<ListResult>(&r)) {
                this->updateTable(std::move(*list));
                std::vector<float> &&fTable = getTableCopyFloat(false);
                callBack = [this, fTable = std::move(fTable)]() mutable {
                    uiState.errMsg = "Success";
                    uiState.currentTable = std::move(fTable);
                    tryUpdateMainUi();
                };
            }
        }
        catch (PyException &e) {
            callBack = [this, msg = std::string(e.what())] {
                uiState.errMsg = msg;
                tryUpdateMainUi();
            };
        }
        MsgMan::callAsync([callBack = std::move(callBack), this] {
            callBack();
            uiState.isButtonEnable = true;
            if (getMainUi() != nullptr) {
                tryUpdateMainUi();
            }
        });
    });
}

void Controller::updateTable(std::vector<double> &&_table) {
    std::lock_guard lg(lock);
    table = std::move(_table);
    _isChanged = true;
}

std::vector<float> Controller::getTableCopyFloat(bool setChangedToFalse) {
    std::lock_guard lg(lock);
    if (table.empty()) {
        return {};
    }
    if (setChangedToFalse) {
        _isChanged = false;
    }
    std::vector<float> newTable;
    newTable.reserve(table.size());
    for (double i: table) {
        newTable.push_back(static_cast<float>(i));
    }
    return newTable;
}

std::vector<float> Controller::tryGetTableCopy() {
    if (isChanged()) {
        return getTableCopyFloat(true);
    }
    else {
        return {};
    }
}

bool Controller::isChanged() {
    std::lock_guard lg(lock);
    return _isChanged;
}

std::unique_ptr<juce::XmlElement> Controller::serializeState() {
    juce::ValueTree tree{"STATE"};
    tree.appendChild(apvts.copyState(), nullptr);
    tree.setProperty("PYCODE", juce::String(uiState.code), nullptr);
    return tree.createXml();
}

void Controller::restoreState(juce::XmlElement &xmlTree) {
    auto tree = juce::ValueTree::fromXml(xmlTree);
    auto newState = tree.getChild(0);
    apvts.replaceState(newState);
    if (tree.hasProperty("PYCODE")) {
        auto newCode = juce::String(tree.getProperty("PYCODE")).toStdString();
        uiState.code = newCode;
        auto ui = getMainUi();
        if (ui != nullptr && uiState.code != newCode) {
            MsgMan::callAsync([this, code = std::move(newCode)] {
                tryUpdateMainUi();
            });
        }
    }
}

APVTS *Controller::getApvts() { return &apvts; }

}// namespace ZX