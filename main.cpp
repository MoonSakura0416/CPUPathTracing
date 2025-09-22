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
#include "Material/dielectric.h"
#include "Material/ground.h"
#include "Material/conductor.h"

int main()
{
    Film   film{192 * 4, 108 * 4};
    Camera camera{film, {-10, 1.5, 0}, {0, 0, 0}, 45};

    Scene scene{};
    auto  sphere = std::make_shared<Sphere>(glm::vec3{0, 0, 0}, 1.f);
    auto  plane = std::make_shared<Plane>(Plane{{0, 0, 0}, {0, 1, 0}});
    auto  model = std::make_shared<Model>("model/dragon.obj");
    RNG   rng{1234};

    for (int i = -3; i <=3; i++) {
        scene.addShape(sphere, std::make_shared<Dielectric>(1.f+0.2f*(i+3), glm::vec3{1,1,1}),
            {0,0.5,i*2}, {0.8,0.8,0.8});
    }

    for (int i = -3; i <=3; i++) {
        glm::vec3 c = RGB::GenerateHeatMapRGB((i+3.f)/6.f);
        scene.addShape(sphere, std::make_shared<Conductor>(glm::vec3{2.f - c *2.f}, glm::vec3{2.f+c*3.f}),
            {0,2.5,i*2}, {0.8,0.8,0.8});
    }

    scene.addShape(model, std::make_shared<Dielectric>(1.8, RGB{128, 191, 131}), {-5,0.4,1.5}, {2,2,2});
    scene.addShape(model, std::make_shared<Conductor>(glm::vec3{0.1, 1.2, 1.8}, glm::vec3{5,2.5,2}), {-5,0.4,-1.5}, {2,2,2});

    scene.addShape(plane, std::make_shared<Ground>(RGB(120, 204, 157)), {0, -0.5, 0});
    auto lightMaterial = std::make_shared<Diffuse>(glm::vec3{1,1,1});
    lightMaterial->setEmission({0.95,0.95,1.f});
    scene.addShape(plane,lightMaterial, {0,10,0});
    scene.build();

    NormalRenderer normalRenderer{camera, scene};
    normalRenderer.render(1, "normal.ppm");

    AABBTestCountRenderer aabbTestCountRenderer{camera, scene};
    aabbTestCountRenderer.render(1, "atc.ppm");
    TriTestCountRenderer triTestCountRenderer{camera, scene};
    triTestCountRenderer.render(1, "ttc.ppm");

    PathTracingRenderer pathTracingRenderer{camera, scene};
    pathTracingRenderer.render(128, "PT_test2.ppm");
}