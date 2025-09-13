#pragma once

#include "spin_lock.h"

#include <functional>
#include <vector>
#include <thread>
#include <list>

class Task {
public:
    virtual ~Task() = default;
    virtual void run() = 0;
};

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads = 0);
    ~ThreadPool();

    void wait() const;

    static void workerThread(ThreadPool* master);
    void addTask(Task* task);
    Task* getTask();

    void parallelFor(size_t width, size_t height, const std::function<void(size_t, size_t)>& func);

private:
    std::atomic<bool> alive_ {true};
    std::vector<std::thread> threads_;
    std::list<Task*> tasks_;
    SpinLock spinlock_{};
};
