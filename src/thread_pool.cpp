#include "thread_pool.h"

ThreadPool::ThreadPool(size_t numThreads)
{
    if (numThreads == 0) {
        numThreads = std::thread::hardware_concurrency();
    }
    for (size_t i = 0; i < numThreads; i++) {
        threads_.emplace_back(std::thread(ThreadPool::workerThread, this));
    }
}

ThreadPool::~ThreadPool()
{
    while (!tasks_.empty()) {
        std::this_thread::yield();
    }
    alive_ = false;
    for (auto& thread : threads_) {
        thread.join();
    }
    threads_.clear();
}

void ThreadPool::workerThread(ThreadPool* master)
{
    while (master->alive_) {
        if (auto task = master->getTask(); task != nullptr) {
            task->run();
        } else {
            std::this_thread::yield();
        }
    }
}

void  ThreadPool::addTask(Task* task)
{
    std::lock_guard<std::mutex> lock(mutex_);
    tasks_.push_back(task);
}

Task* ThreadPool::getTask()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (tasks_.empty()) {
        return nullptr;
    }
    Task* task = tasks_.front();
    tasks_.pop_front();
    return task;
}
