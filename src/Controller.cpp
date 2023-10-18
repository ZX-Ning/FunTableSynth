#include "Controller.h"

#include "AsyncThread.h"
#include "PluginProcessor.h"
#include "PyInterpreter.h"
#include "ui/PluginEditor.h"
#include <algorithm>
#include <future>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <variant>
#include <vector>

namespace ZX {
const char *Controller::defaultCode = "#Enter expression here using python syntax\n"
                                      "#Your code in here will be warpped in a python function\n"
                                      "#and the parameter is x(already imported * from math)\n\n"
                                      "return sin(pi * x)";

const char *Controller::defaultErrmsg = "Welcome to FUN SHAPER!\n"
                                        "Enter expression above using python syntax and hit apply button to perform calculation.\n"
                                        "Notice: The calculation task will be automatically stopped if it runs so long"
                                        "(current time limit is set to 5 second)";

const int TIME_OUT_MS = 5000;

using MsgMan = juce::MessageManager;
using ListResult = PyInterpreter::ListResult;

static APVTS::ParameterLayout getParameters() {
    return APVTS::ParameterLayout{
            std::make_unique<juce::AudioParameterFloat>(
                    juce::ParameterID("GAIN", 1),
                    "Gain", -48.0f, 0.0f, 0.0f),
            std::make_unique<juce::AudioParameterBool>(
                    juce::ParameterID("DC_OFFSET_REMOVE", 1),
                    "remove DC offset", false),
            std::make_unique<juce::AudioParameterBool>(
                    juce::ParameterID("USING_LOOKUP", 1),
                    "using lookup table", true)};
}

MainUi *Controller::getMainUi() const {
    auto *editor = processor->getActiveEditor();
    if (editor == nullptr) {
        return nullptr;
    }
    auto *pEditor = dynamic_cast<AudioPluginAudioProcessorEditor *>(editor);
    return pEditor->getMainUi();
}

Controller::Controller(AudioPluginAudioProcessor *_processor)
    : processor(_processor), apvts(*_processor, nullptr,
                                   "PARAMETERS", getParameters()) {
}

bool Controller::setPyCode() {
    try {
        std::stringstream iss{getMainUi()->getCodeInside()};
        std::stringstream oss;
        oss << "def osc(x,p):\n";
        while (iss.good()) {
            char buf[100];
            iss.getline(buf, 100);
            oss << "   " << buf << "\n";
        }
        oss << std::endl;
//        juce::Logger::outputDebugString(oss.str());
        py.addFunction(oss.str().c_str());
        getMainUi()->updateErrorMsg("Successfully compiled code");
        return true;
    }
    catch (PyException &e) {
        getMainUi()->updateErrorMsg(e.what());
        return false;
    }
}

void Controller::setTimeOut(double _timeOut) { timeOut = _timeOut; }

void Controller::startCalc(double f, double t, double step, double p) {
    if (py.isRunning()) {
        getMainUi()->updateErrorMsg("Error: already running!");
        return;
    }
    getMainUi()->updateButtonEnable(false);
    auto asyncRunner = AsyncThread::getAsyncRunner();
    getMainUi()->updateErrorMsg("Calculating...");
    asyncRunner->asyncRun([f, t, step, p, this] {
        std::function<void()> callBack;
        try {
            this->py.startCalc(f, t, step, p);
//            std::cout << "started" << std::endl;
            auto r = this->py.waitAndGet(TIME_OUT_MS);
            if (auto *str = std::get_if<std::string>(&r)) {
                std::string &errmsg = *str;
                callBack = [this, errmsg = std::move(errmsg)] {
                    this->getMainUi()->updateErrorMsg(errmsg.c_str());
                };
            }
            else if (auto *list = std::get_if<ListResult>(&r)) {
                this->updateTable(std::move(*list));
                std::vector<float> &&fTable = getTableCopyFloat(false);
//                std::cout << "Success thread: " << std::this_thread::get_id() << std::endl;
//                ZX::_debug_printResult(fTable);
                callBack = [this, fTable = std::move(fTable)]() mutable {
                    this->getMainUi()->updateErrorMsg("Success");
                    this->getMainUi()->updateGraph(std::move(fTable));
                };
            }
        }
        catch (PyException &e) {
            callBack = [this, msg = std::string(e.what())] {
                this->getMainUi()->updateErrorMsg(msg.c_str());
            };
        }
        MsgMan::callAsync([callBack = std::move(callBack), this] {
            if (getMainUi() != nullptr) {
                callBack();
                this->getMainUi()->updateButtonEnable(true);
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
    if (table.empty()){
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

bool Controller::isChanged() {
    std::lock_guard lg(lock);
    return _isChanged;
}

std::unique_ptr<juce::XmlElement> Controller::serializeState() {
    juce::ValueTree tree{"STATE"};
    tree.appendChild(apvts.copyState(), nullptr);
    if (getMainUi() != nullptr) {
        tree.setProperty("PYCODE", juce::String(getMainUi()->getCodeInside().c_str()), nullptr);
    }
    return tree.createXml();
}

void Controller::restoreState(juce::XmlElement &xmlTree) {
    auto tree = juce::ValueTree::fromXml(xmlTree);
    auto newState = tree.getChild(0);
    apvts.replaceState(newState);
    if (tree.hasProperty("PYCODE")) {
        auto newCode = juce::String(tree.getProperty("PYCODE")).toStdString();
        auto ui = getMainUi();
        if (ui != nullptr && std::string(ui->getCodeInside()) != newCode) {
            MsgMan::callAsync([this, code = std::move(newCode)] {
                this->getMainUi()->updateCodeEditor(code.c_str());
            });
        }
    }
}

APVTS *Controller::getApvts() { return &apvts; }

}// namespace ZX