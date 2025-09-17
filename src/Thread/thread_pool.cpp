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
    ParallelForTask(size_t x, size_t y, size_t chunkWidth, size_t chunkHeight,
                    const std::function<void(size_t, size_t)>& func)
        : x_(x), y_(y), chunkWidth_(chunkWidth), chunkHeight_(chunkHeight), func_(func)
    {
    }

    void run() override
    {
        for (size_t i = 0; i < chunkWidth_; ++i)
            for (size_t j = 0; j < chunkHeight_; ++j) {
                func_(x_ + i, y_ + j);
            }
    }

private:
    size_t                              x_, y_, chunkWidth_, chunkHeight_;
    std::function<void(size_t, size_t)> func_;
};

void ThreadPool::parallelFor(size_t width, size_t height,
                             const std::function<void(size_t, size_t)>& func, bool complex)
{
    //PROFILE(__func__)

    float chunkWidthFloat = static_cast<float>(width) / std::sqrt(threads_.size());
    float chunkHeightFloat =
        static_cast<float>(height) / std::sqrt(threads_.size());
    if (complex) {
        chunkWidthFloat /= std::sqrt(16);
        chunkHeightFloat /= std::sqrt(16);
    }
    auto chunkWidth = static_cast<size_t>(std::ceil(chunkWidthFloat));
    auto chunkHeight = static_cast<size_t>(std::ceil(chunkHeightFloat));

    for (size_t i = 0; i < width; i += chunkWidth) {
        const size_t currW = std::min(chunkWidth, width - i);
        for (size_t j = 0; j < height; j += chunkHeight) {
            {
                const size_t currH = std::min(chunkHeight, height - j);
                Guard guard{spinlock_};
                ++pendingTasks_;
                tasks_.push(std::make_unique<ParallelForTask>(i, j, currW, currH, func));
            }
        }
    }
}