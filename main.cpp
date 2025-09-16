#include "Thread/thread_pool.h"
#include "Camera/film.h"
#include "Camera/camera.h"
#include "Shape/sphere.h"
#include "Shape/model.h"
#include "Shape/plane.h"
#include "Shape/scene.h"
#include "Util/frame.h"
#include "Util/rgb.h"
#include "Util/constants.h"
#include "Util/rng.h"

#include <print>
#include <chrono>
#include <memory>
#include <random>

inline glm::vec3 sampleCosineHemisphere(float u1, float u2)
{
    // Malley: r = sqrt(u1), phi = 2π u2
    float r = std::sqrt(u1);
    float phi = 2.0f * Pi * u2;
    float x = r * std::cos(phi);
    float z = r * std::sin(phi);
    float y = std::sqrt(std::max(0.0f, 1.0f - u1));
    return {x, y, z};
}

int main()
{
    ThreadPool threadpool{};
    Film       film{192 * 4, 108 * 4};
    Camera     camera{film, {-3.6f, 0, 0}, {0, 0, 0}, 45};

    Scene scene{};
    auto  sphere = std::make_shared<Sphere>(glm::vec3{0, 0, 0}, 1.f);
    auto  plane = std::make_shared<Plane>(Plane{{0, 0, 0}, {0, 1, 0}});
    scene.addShape(std::make_shared<Model>("model/simple_dragon.obj"),
                   std::make_shared<Material>(RGB(202, 159, 117)), {0, 0, 0}, {1, 3, 2});
    scene.addShape(sphere,
                   std::make_shared<Material>(glm::vec3{1, 1, 1}, false, RGB(255, 128, 128)),
                   {0, 0, 2.5});
    scene.addShape(sphere,
                   std::make_shared<Material>(glm::vec3{1, 1, 1}, false, RGB(128, 128, 255)),
                   {0, 0, -2.5});
    scene.addShape(sphere, std::make_shared<Material>(glm::vec3{1, 1, 1}, true), {3, 0.5, -2});
    scene.addShape(plane, std::make_shared<Material>(RGB(120, 204, 157)), {0, -0.5, 0});

    std::atomic<int> count{0};

    // std::mt19937                          gen{};
    // std::uniform_real_distribution<float> uniform{0.f, 1.f};

    int spp = 50;

    auto start = std::chrono::high_resolution_clock::now();

    threadpool.parallelFor(
        film.getWidth(), film.getHeight(), [&, baseSeed = 1337u](size_t x, size_t y) {
            thread_local RNG  rng;
            thread_local bool inited = false;
            if (!inited) {
                auto          tid = std::hash<std::thread::id>{}(std::this_thread::get_id());
                std::uint64_t seed = 0x9E3779B97F4A7C15ull ^ static_cast<std::uint64_t>(baseSeed) ^
                                     (tid + (tid << 6) + (tid >> 2));
                rng.setSeed(static_cast<size_t>(seed));
                inited = true;
            }
            for (int i = 0; i < spp; ++i) {
                auto      ray = camera.getRay({x, y}, {rng.uniform(), rng.uniform()});
                glm::vec3 beta = {1, 1, 1};
                glm::vec3 color = {0, 0, 0};
                while (true) {
                    auto hitInfo = scene.intersect(ray);
                    if (hitInfo.has_value()) {
                        color += beta * hitInfo->material->emissive;
                        beta *= hitInfo->material->albedo;

                        ray.origin = hitInfo->hitPos;
                        glm::vec3 lightDir;
                        Frame     frame{hitInfo->normal};
                        if (hitInfo->material->isSpecular) {
                            glm::vec3 viewDir = frame.localFromWorld(-ray.direction);
                            lightDir = {-viewDir.x, viewDir.y, -viewDir.z};
                            // ray.direction =
                            //     glm::reflect(ray.direction, hitInfo->normal);
                        } else {
                            do {
                                lightDir = {rng.uniform(), rng.uniform(), rng.uniform()};
                                lightDir = lightDir * 2.f - 1.f;
                            } while (glm::length(lightDir) > 1.f);
                            if (lightDir.y < 0) {
                                lightDir.y = -lightDir.y;
                            }
                            // float     u1 = uni(rng);
                            // float     u2 = uni(rng);
                            // glm::vec3 wiLocal = sampleCosineHemisphere(u1, u2);
                            // glm::vec3 wi = frame.worldFromLocal(wiLocal);
                            // ray.direction = wi;
                        }
                        ray.direction = frame.worldFromLocal(lightDir);
                    } else {
                        break;
                    }
                }
                film.addSample(x, y, color);
                // film.setPixel(x, y, {1, 1, 1});
            }

            ++count;
            if (count % film.getWidth() == 0) {
                std::println("{:.2f}",
                             static_cast<float>(count) / (film.getWidth() * film.getHeight()));
            }
        });

    threadpool.wait();

    film.save("test4.ppm");
    auto end = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::println("Time: {}", diff);
}
