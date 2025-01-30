//
// Created by simeon on 22.01.25.
//

#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

template<typename T>
class ThreadSafeQueue {
public:
    void enqueue(const T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(value);
    }

    std::optional<T> dequeue() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) return std::nullopt;
        T value = queue_.front();
        queue_.pop();
        return value;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }
private:
    mutable std::mutex mutex_;
    std::queue<T> queue_;
};



#endif //THREADSAFEQUEUE_H
