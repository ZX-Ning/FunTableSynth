#ifndef ZX_PY_INTERPRETER_H_
#define ZX_PY_INTERPRETER_H_

#include <Python.h>
#include <exception>
#include <memory>
#include <string>
#include <variant>
#include <vector>
#include <iostream>


namespace ZX {

struct PyGilGuard final {
    PyGILState_STATE gilState;
    bool working;

    explicit PyGilGuard();
    ~PyGilGuard();
};

struct PyObjDeleter final {
    void operator()(PyObject *);
};

struct PyException : public std::exception {
    std::string msg;

    explicit PyException(const char *);
    [[nodiscard]] const char *what() const noexcept override;
};


using PyObjPtr = std::unique_ptr<PyObject, PyObjDeleter>;

class PyInterpreter final {
private:
    const static char *pyCode;
    PyObjPtr pMainModule;
    PyObjPtr pGlobalDict;
    PyThreadState *_save;
    bool _isRunning;

    inline void throwIfNull(void *, const char *) noexcept(false);
    static std::vector<double> getPyFloatList(PyObject *);
    std::string getAndGenErrMsg(const char *where) noexcept;
    PyObject *getObject(const char *name);

public:
    using ListResult = std::vector<double>;
    using ResultType = std::variant<std::string, ListResult>;
    explicit PyInterpreter();
    PyInterpreter(PyInterpreter &) = delete;
    PyInterpreter(PyInterpreter &&) = delete;
    ~PyInterpreter();
    void addFunction(const char *code);
    double calcSingle(double, double);
    void startCalc(double, double, double, double);
    void stopCalc();
    bool isRunning() const;
    ResultType waitAndGet(double ms);
};

template<typename T>
void _debug_printResult(const std::vector<T> &resultList) {
    std::cout<<"[";
    for (auto it = resultList.begin(); it != resultList.end(); ++it) {
        std::cout << *it
                  << (it == resultList.end() - 1 ? " " : ", ");
    }
    std::cout<<"]"<<std::endl;
}

}// namespace ZX

#endif//#ifndef ZX_PY_INTERPRETER_H_