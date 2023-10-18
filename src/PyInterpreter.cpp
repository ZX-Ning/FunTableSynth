#include "PyInterpreter.h"
#include "BinaryData.h"
#include <optional>
#include <sstream>

namespace ZX {

PyGilGuard::PyGilGuard() {
    if (!PyGILState_Check()) {
        gilState = PyGILState_Ensure();
        working = true;
    }
    else {
        working = false;
    }
}

PyGilGuard::~PyGilGuard() {
    if (working) {
        PyGILState_Release(gilState);
    }
}

void PyObjDeleter::operator()(PyObject *obj) {
    if (obj != nullptr) {
        std::unique_ptr<PyGilGuard> gilGuardPtr = nullptr;
        if (!PyGILState_Check()) {
            gilGuardPtr = std::make_unique<PyGilGuard>();
        }
        Py_XDECREF(obj);
    }
}

PyException::PyException(const char *_msg) : msg(_msg) {}

const char *PyException::what() const noexcept {
    return msg.c_str();
}

void PyInterpreter::throwIfNull(void *p, const char *where) noexcept(false) {
    if (p == nullptr) {
        throw PyException(getAndGenErrMsg(where).c_str());
    }
}

std::string PyInterpreter::getAndGenErrMsg(const char *where) noexcept {
    PyObject * p1, *p2, *p3;
    PyErr_Fetch(&p1, &p2, &p3);
    PyObjPtr pType(p1), pValue(p2), pTraceback(p3);
    std::stringstream msgStream;
    msgStream << "Error in Python code when " << where << ":";
    if (pValue == nullptr) {
        msgStream << "no Python exception caught" << std::endl;
    }
    else {
        PyObjPtr pStrValue{PyObject_Str(pValue.get())};
        const char *errorMsg = PyUnicode_AsUTF8(pStrValue.get());
        msgStream << "\n   " << errorMsg << "\n";
        if (pTraceback != nullptr) {
            PyObjPtr tb{PyObject_CallOneArg(getObject("cppFormatTB"), pTraceback.get())};
            msgStream << "TraceBack: \n" << PyUnicode_AsUTF8(tb.get()) << std::endl;
        }
    }
    return msgStream.str();
}


PyInterpreter::PyInterpreter() : _isRunning(false) {
    Py_Initialize();
    pMainModule.reset(PyImport_AddModule("__main__"));
    pGlobalDict.reset(PyModule_GetDict(pMainModule.get()));
    Py_INCREF(pGlobalDict.get());
    PyObjPtr pCode{Py_CompileStringExFlags(BinaryData::internal_py, "internal_py_code",
                                           Py_file_input, nullptr, 2)};
    throwIfNull(pCode.get(), "compile");
    PyObjPtr pEval{PyEval_EvalCode(pCode.get(), pGlobalDict.get(), pGlobalDict.get())};
    throwIfNull(pCode.get(), "compile");
    _save = PyEval_SaveThread();
}

PyInterpreter::~PyInterpreter() {
    PyEval_RestoreThread(_save);
    if (isRunning()) {
        stopCalc();
    }
    Py_Finalize();
}

std::vector<double> PyInterpreter::getPyFloatList(PyObject *pList) {
    PyGilGuard gilGuard;
    std::vector<double> result;
    auto listSize = static_cast<long>(PyList_Size(pList));
    for (long i = 0; i < listSize; ++i) {
        PyObject * pListItem = PyList_GetItem(pList, i);
        if (!PyFloat_Check(pListItem)) {
            throw PyException("Error: the osc function must return a float number");
        }
        result.push_back(PyFloat_AsDouble(pListItem));
    }
    return result;
}

PyObject *PyInterpreter::getObject(const char *name) {
    PyGilGuard gilGuard;
    PyObject * obj = PyDict_GetItemString(pGlobalDict.get(), name);
//    Py_INCREF(obj);
    return obj;
}

void PyInterpreter::addFunction(const char *code) {
    if (_isRunning) {
        stopCalc();
    }
    PyGilGuard guard;
    if (PyDict_GetItemString(pGlobalDict.get(), "osc") != nullptr) {
        PyDict_DelItemString(pGlobalDict.get(), "osc");
    }
    PyObjPtr pCode{Py_CompileStringExFlags(code, "user_input_code", Py_file_input, nullptr, 2)};
    throwIfNull(pCode.get(), "compile");
    PyObjPtr pEval{PyEval_EvalCode(pCode.get(), pGlobalDict.get(), pGlobalDict.get())};
    throwIfNull(pCode.get(), "Eval code");
}

double PyInterpreter::calcSingle(double x, double p) {
    PyGilGuard gilGuard;
    PyObjPtr pResult{PyObject_CallFunction(getObject("osc"),
                                           "(dd)", x, p)};
    throwIfNull(pResult.get(), "starting osc function");
    if (!PyFloat_Check(pResult.get())) {
        throw PyException("Error: the osc function must return a float number");
    }
    return PyFloat_AsDouble(pResult.get());
}

void PyInterpreter::startCalc(double f, double t, double step, double p) {
    PyGilGuard gilGuard;
    PyObjPtr pResult{PyObject_CallFunction(getObject("cppStart"), "(dddd)", f, t, step, p)};
    throwIfNull(pResult.get(), "Start calc job");
    if (PyUnicode_Check(pResult.get())) {
        throw PyException(PyUnicode_AsUTF8(pResult.get()));
    }
    _isRunning = true;
}

void PyInterpreter::stopCalc() {
    _isRunning = false;
    PyGilGuard gilGuard;
    PyObjPtr result{PyObject_CallNoArgs(getObject("cppStop"))};
    if (PyUnicode_Check(result.get())) {
        throw PyException(PyUnicode_AsUTF8(result.get()));
    }
}

PyInterpreter::ResultType PyInterpreter::waitAndGet(double ms) {
    PyGilGuard gilGuard;
    PyObjPtr resultAPtr{PyObject_CallFunction(getObject("cppWait"), "(d)", ms)};
    PyObject * result = resultAPtr.get();
    throwIfNull(result, "Wait and get result");
    _isRunning = false;
    if (PyUnicode_Check(result)) {
        return std::string{PyUnicode_AsUTF8(result)};
    }
    else {
        return getPyFloatList(result);
    }
}

bool PyInterpreter::isRunning() const { return _isRunning; }
}// namespace ZX