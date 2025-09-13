#include "thread_pool.h"

ThreadPool::ThreadPool(size_t numThreads)
{
    if (numThreads == 0) {
        numThreads = std::thread::hardware_concurrency();
    }
    for (size_t i = 0; i < numThreads; i++) {
        threads_.push_back(std::thread(ThreadPool::workerThread, this));
    }
}

ThreadPool::~ThreadPool()
{
    wait();
    alive_ = false;
    for (auto& thread : threads_) {
        thread.join();
    }
    threads_.clear();
}
void ThreadPool::wait() const
{
    while (!tasks_.empty()) {
        std::this_thread::yield();
    }
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

void ThreadPool::addTask(Task* task)
{
    Guard guard{spinlock_};
    tasks_.push_back(task);
}

Task* ThreadPool::getTask()
{
    Guard guard{spinlock_};
    if (tasks_.empty()) {
        return nullptr;
    }
    Task* task = tasks_.front();
    tasks_.pop_front();
    return task;
}

class ParallelForTask : public Task {
public:
    ParallelForTask(size_t x, size_t y, const std::function<void(size_t, size_t)>& func)
        : x_(x), y_(y), func_(func)
    {
    }

    void run() override
    {
        func_(x_, y_);
    }

private:
    size_t                              x_, y_;
    std::function<void(size_t, size_t)> func_;
};

void ThreadPool::parallelFor(size_t width, size_t height,
                             const std::function<void(size_t, size_t)>& func)
{
    Guard guard{spinlock_};
    for (size_t i = 0; i < width; i++) {
        for (size_t j = 0; j < height; j++) {
            tasks_.push_back(new ParallelForTask(i, j, func));
        }
    }
}
