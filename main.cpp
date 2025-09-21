#include "Thread/thread_pool.h"
#include "Camera/film.h"
#include "Camera/camera.h"
#include "Shape/sphere.h"
#include "Shape/model.h"
#include "Shape/plane.h"
#include "Shape/scene.h"
#include "Util/rgb.h"
#include "Renderer/normal_renderer.h"
#include "Renderer/debug_renderer.h"
#include "Renderer/path_tracing_renderer.h"
#include "Material/diffuse.h"
#include "Material/specular.h"

int main()
{
    Film   film{192 * 4, 108 * 4};
    Camera camera{film, {-12, 5, -12}, {0, 0, 0}, 45};

    Scene scene{};
    auto  sphere = std::make_shared<Sphere>(glm::vec3{0, 0, 0}, 1.f);
    auto  plane = std::make_shared<Plane>(Plane{{0, 0, 0}, {0, 1, 0}});
    auto  model = std::make_shared<Model>("model/dragon.obj");
    RNG   rng{1234};
    for (int i = 0; i < 10000; i++) {
        glm::vec3 randomPos{
            rng.uniform() * 100 - 50,
            rng.uniform() * 2,
            rng.uniform() * 100 - 50,
        };
        float u = rng.uniform();
        if (u < 0.9) {
            std::shared_ptr<Material> material;
            if (rng.uniform() > 0.5) {
                material = std::make_shared<Specular>(RGB(202, 159, 117));
            } else {
                material = std::make_shared<Diffuse>(RGB(202, 159, 117));
            }
            scene.addShape(model, material, randomPos, {1, 1, 1},
                           {rng.uniform() * 360, rng.uniform() * 360, rng.uniform() * 360});
        } else if (u < 0.95) {
            scene.addShape(
                sphere,
                std::make_shared<Specular>(glm::vec3{rng.uniform(), rng.uniform(), rng.uniform()}),
                randomPos, {0.4, 0.4, 0.4});
        } else {
            randomPos.y += 6;
            auto material = std::make_shared<Diffuse>(glm::vec3{0, 0, 0});
            material->setEmission({rng.uniform() * 4, rng.uniform() * 4, rng.uniform() * 4});
            scene.addShape(sphere, material, randomPos);
        }
    }

    scene.addShape(plane, std::make_shared<Diffuse>(RGB(120, 204, 157)), {0, -0.5, 0});
    scene.build();

    NormalRenderer normalRenderer{camera, scene};
    normalRenderer.render(1, "normal.ppm");

    AABBTestCountRenderer aabbTestCountRenderer{camera, scene};
    aabbTestCountRenderer.render(1, "atc.ppm");
    TriTestCountRenderer triTestCountRenderer{camera, scene};
    triTestCountRenderer.render(1, "ttc.ppm");

    PathTracingRenderer pathTracingRenderer{camera, scene};
    pathTracingRenderer.render(128, "PT_test.ppm");
}