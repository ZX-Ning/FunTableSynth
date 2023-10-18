#ifndef ZX_ASYNCTHREAD_H
#define ZX_ASYNCTHREAD_H

#include <optional>
#include <thread>
#include "ThreadSafeQueue.h"


namespace ZX {

class AsyncThread {
private:
    using Action = std::function<void(void)>;
    using EventType = std::optional<Action>;
    static std::unique_ptr<AsyncThread> _instance;
    AsyncThread();
    ThreadSafeQueue<EventType> events;
    std::thread t;
public:
    ~AsyncThread();
    AsyncThread(AsyncThread&) = delete;
    AsyncThread(AsyncThread&&) = delete;
    static AsyncThread *getAsyncRunner();
    void asyncRun(Action);
    void stop();
};

}


#endif //ZX_ASYNCTHREAD_H
