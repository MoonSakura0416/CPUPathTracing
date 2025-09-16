#include "Thread/thread_pool.h"
#include "Util/profile.h"

ThreadPool::ThreadPool(size_t numThreads)
{
    if (numThreads == 0) {
        numThreads = std::thread::hardware_concurrency();
    }
    for (size_t i = 0; i < numThreads; i++) {
        threads_.emplace_back(workerThread, this);
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
    while (pendingTasks_ > 0) {
        std::this_thread::yield();
    }
}

void ThreadPool::workerThread(ThreadPool* master)
{
    while (master->alive_) {
        if (auto task = master->getTask(); task != nullptr) {
            task->run();
            --(master->pendingTasks_);
        } else {
            std::this_thread::yield();
        }
    }
}

void ThreadPool::addTask(std::unique_ptr<Task> task)
{
    Guard guard{spinlock_};
    ++pendingTasks_;
    tasks_.push(std::move(task));
}

std::unique_ptr<Task> ThreadPool::getTask()
{
    Guard guard{spinlock_};
    if (tasks_.empty()) {
        return nullptr;
    }
    auto task = std::move(tasks_.front());
    tasks_.pop();
    return task;
}

class ParallelForTask final : public Task {
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
    PROFILE(__func__)
    Guard guard{spinlock_};
    for (size_t i = 0; i < width; i++) {
        for (size_t j = 0; j < height; j++) {
            ++pendingTasks_;
            tasks_.push(std::make_unique<ParallelForTask>(i, j, func));
        }
    }
}
