#ifndef ZX_THREAD_SAFE_QUEUE_H_
#define ZX_THREAD_SAFE_QUEUE_H_

#include <mutex>
#include <queue>
#include <stdexcept>

namespace ZX {

template<typename T>
class ThreadSafeQueue {
private:
    std::queue<T> que;
    std::mutex mutexLock;

public:
    void push(T &t) {
        std::lock_guard lg(mutexLock);
        que.push(t);
    }

    void push(T &&t) {
        std::lock_guard lg(mutexLock);
        que.push(std::forward<T &&>(t));
    }

    bool isEmpty() {
        std::lock_guard lg(mutexLock);
        return que.empty();
    }

    T pop() {
        std::lock_guard lg(mutexLock);
        if (que.empty()) {
            throw std::out_of_range("Pop while queue is empty");
        }
        T t{std::move(que.front())};
        que.pop();
        return t;
    }
};

}// namespace ZX

#endif//ZX_THREAD_SAFE_QUEUE_H_