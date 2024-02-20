#ifndef ZX_ASYNCTHREAD_H
#define ZX_ASYNCTHREAD_H

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <functional>


namespace ZX {

class AsyncThread {
private:
    using Action = std::function<void(void)>;
    using EventType = std::optional<Action>;
    static std::unique_ptr<AsyncThread> instance;
    AsyncThread();
    std::queue<EventType> events;
    std::mutex lock;
    std::condition_variable cv;
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
