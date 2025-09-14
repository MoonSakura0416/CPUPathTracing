#include <print>
#include <chrono>

#include "thread_pool.h"
#include "film.h"
#include "camera.h"
#include "sphere.h"

int main()
{
    ThreadPool threadpool{};
    Film film{1920,1080};
    Camera camera{film, {0,0,1}, {0,0,0}, 90};

    Sphere sphere{.center={0,0,0}, .radius=0.5f};

    glm::vec3 lightPos {1,1,1};

    threadpool.parallelFor(film.getWidth(),film.getHeight(),[&film, &camera, &sphere, lightPos](size_t x, size_t y){
        auto ray = camera.getRay({static_cast<int>(x),static_cast<int>(y)});
        auto intersect = sphere.intersect(ray);
        if (intersect.has_value()) {
            auto hitPos = ray.at(intersect.value());
            auto normal = glm::normalize(hitPos - sphere.center);
            auto lightDir = glm::normalize(lightPos - hitPos);
            auto cosine = glm::dot(normal, lightDir);
            film.setPixel(x,y,{cosine,cosine,cosine});
        }

    });

    threadpool.wait();

    auto start = std::chrono::high_resolution_clock::now();
    film.save("test.ppm");
    auto end = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::println("Time: {}", diff);
}