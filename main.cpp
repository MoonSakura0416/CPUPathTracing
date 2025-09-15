#include <print>
#include <chrono>

#include "thread_pool.h"
#include "film.h"
#include "camera.h"
#include "sphere.h"
#include "model.h"

int main()
{
    ThreadPool threadpool{};
    Film       film{1920, 1080};
    Camera     camera{film, {-1, 0, 0}, {0, 0, 0}, 90};

    Model model{"model/simple_dragon.obj"};

    std::atomic<int> count{0};

    Sphere sphere{{0, 0, 0}, 0.5f};

    glm::vec3 lightPos{-1, 2, 1};

    Shape& shape = model;

    threadpool.parallelFor(film.getWidth(), film.getHeight(),
                           [&film, &camera, &shape, &count, lightPos](size_t x, size_t y) {
                               auto ray = camera.getRay({static_cast<int>(x), static_cast<int>(y)});
                               auto hitInfo = shape.intersect(ray);
                               if (hitInfo.has_value()) {
                                   auto lightDir = glm::normalize(lightPos - hitInfo->hitPos);
                                   auto cosine = glm::max(0.f, glm::dot(hitInfo->normal, lightDir));
                                   film.setPixel(x,y,{cosine,cosine,cosine});
                                   //film.setPixel(x, y, {1, 1, 1});
                               }

                               ++count;
                               if (count % film.getWidth() == 0) {
                                   std::println("{:.2f}", static_cast<float>(count) /
                                                              (film.getWidth() * film.getHeight()));
                               }
                           });

    threadpool.wait();

    auto start = std::chrono::high_resolution_clock::now();
    film.save("test.ppm");
    auto end = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::println("Time: {}", diff);
}