#include "AsyncThread.h"

#include <memory>
#include <mutex>
#include <utility>

namespace ZX {

std::unique_ptr<AsyncThread> AsyncThread::instance{nullptr};

AsyncThread::AsyncThread() {
    using namespace std::chrono;
    t = std::thread{[this] {
        while (true) {
            std::unique_lock lk(lock);
            cv.wait(lk, [this] { return !events.empty(); });
            auto event(std::move(events.front()));
            events.pop();
            if (event.has_value()) {
                (event.value())();
            }
            else {
                return;
            }
        }
    }};
}

AsyncThread::~AsyncThread() {
    stop();
    t.join();
}

AsyncThread* AsyncThread::getAsyncRunner() {
    if (instance == nullptr) {
        instance.reset(new AsyncThread);
    }
    return instance.get();
}

void AsyncThread::asyncRun(AsyncThread::Action action) { 
    events.push(action); 
    cv.notify_one();
}

void AsyncThread::stop() { 
    events.push(std::nullopt); 
    cv.notify_one();
}

}  // namespace ZX