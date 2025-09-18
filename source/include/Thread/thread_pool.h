#pragma once

#include "pch.h"

#include "spin_lock.h"

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
    void addTask(std::unique_ptr<Task> task);
    std::unique_ptr<Task> getTask();

    void parallelFor(size_t width, size_t height, const std::function<void(size_t, size_t)>& func,
                     bool complex = true);

private:
    std::atomic<bool> alive_ {true};
    std::atomic<int> pendingTasks_ {0};
    std::vector<std::thread> threads_;
    std::queue<std::unique_ptr<Task>> tasks_;
    SpinLock spinlock_{};
};

inline ThreadPool threadpool{};
