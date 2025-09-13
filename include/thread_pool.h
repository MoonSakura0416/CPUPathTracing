#pragma once

#include <vector>
#include <thread>
#include <list>
#include <mutex>

class Task {
public:
    virtual ~Task() = default;
    virtual void run() = 0;
};

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads);
    ~ThreadPool();

    static void workerThread(ThreadPool* master);
    void addTask(Task* task);
    Task* getTask();
private:
    bool alive_ {true};
    std::vector<std::thread> threads_;
    std::list<Task*> tasks_;
    std::mutex mutex_;
};
