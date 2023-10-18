#include "AsyncThread.h"
#include <memory>

namespace ZX{

std::unique_ptr<AsyncThread> AsyncThread::_instance{nullptr};

AsyncThread::AsyncThread(){
    using namespace  std::chrono;
    t = std::thread{[this]{
        while (true){
            if (events.isEmpty()){
                std::this_thread::sleep_for(microseconds(200));
                continue;
            }
            auto event = events.pop();
            if (event.has_value()){
                (event.value())();
            }
            else{
                return ;
            }
        }
    }};
}

AsyncThread::~AsyncThread() {
    stop();
    t.join();
}

AsyncThread* AsyncThread::getAsyncRunner(){
    if (_instance == nullptr) {
        _instance.reset(new AsyncThread);
    }
    return _instance.get();
}

void AsyncThread::asyncRun(AsyncThread::Action action) {
    events.push(action);
}

void AsyncThread::stop() {
    events.push(std::nullopt);
}

}