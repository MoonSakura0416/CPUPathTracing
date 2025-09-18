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
#include "Renderer/normal_renderer.h"
#include "Renderer/simple_rt_renderer.h"
#include "Renderer/debug_renderer.h"


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
    Film       film{192 * 4, 108 * 4};
    Camera     camera{film, {-3.6f, 0, 0}, {0, 0, 0}, 45};

    Scene scene{};
    auto  sphere = std::make_shared<Sphere>(glm::vec3{0, 0, 0}, 1.f);
    auto  plane = std::make_shared<Plane>(Plane{{0, 0, 0}, {0, 1, 0}});
    scene.addShape(std::make_shared<Model>("model/dragon.obj"),
                   std::make_shared<Material>(RGB(202, 159, 117)), {0, 0, 0}, {1, 3, 2});
    scene.addShape(sphere,
                   std::make_shared<Material>(glm::vec3{1, 1, 1}, false, RGB(255, 128, 128)),
                   {0, 0, 2.5});
    scene.addShape(sphere,
                   std::make_shared<Material>(glm::vec3{1, 1, 1}, false, RGB(128, 128, 255)),
                   {0, 0, -2.5});
    scene.addShape(sphere, std::make_shared<Material>(glm::vec3{1, 1, 1}, true), {3, 0.5, -2});
    scene.addShape(plane, std::make_shared<Material>(RGB(120, 204, 157)), {0, -0.5, 0});

    NormalRenderer normalRenderer{camera,scene};
    normalRenderer.render(1, "normal.ppm");


    SimpleRTRenderer simpleRTRenderer{camera,scene};
    simpleRTRenderer.render(128, "dragon_rt.ppm");
    AABBTestCountRenderer aabbTestCountRenderer{camera, scene};
    aabbTestCountRenderer.render(1, "atc.ppm");
    TriTestCountRenderer triTestCountRenderer{camera, scene};
    triTestCountRenderer.render(1, "ttc.ppm");
    AABBTestDepthRenderer aabbTestDepthRenderer{camera, scene};
    aabbTestDepthRenderer.render(1, "atd.ppm");
}
