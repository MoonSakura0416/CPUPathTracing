#include <print>
#include <chrono>

#include "thread_pool.h"
#include "film.h"

class SimpleTask final : public Task {
    void run() override
    {
        std::println("SimpleTask::run()");
    }
};

int main()
{
    Film film{1920,1080};
    for (int j = 0; j < 100; j++) {
        for (int i = 0; i < 200; i++) {
            film.SetPixel(i,j,{0.5,0.7,0.2});
        }
    }

    auto start = std::chrono::high_resolution_clock::now();
    film.Save("test.ppm");
    auto end = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::println("Time: {}", diff);
}