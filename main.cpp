#include <print>
#include <chrono>

#include "thread_pool.h"
#include "film.h"

int main()
{
    ThreadPool threadpool{};
    Film film{1920,1080};
    threadpool.parallelFor(200,100,[&film](size_t x, size_t y){
        film.setPixel(x,y,{0.5,0.7,0.2});
    });

    threadpool.wait();

    auto start = std::chrono::high_resolution_clock::now();
    film.save("test.ppm");
    auto end = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::println("Time: {}", diff);
}