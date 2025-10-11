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
#include "Shape/quad.h"
#include "Renderer/previewer.h"

#include "scene/cornellbox.h"




int main()
{
    // Film   film{192 * 10, 108 * 10};
    // Camera camera{film, {-10, 1.5, 0}, {0, 0, 0}, 45};
    //
    // Scene scene{};
    // auto  sphere = scene.addShape<Sphere>(glm::vec3{0, 0, 0}, 1.f);
    // auto  plane = scene.addShape<Plane>(glm::vec3{0, 0, 0}, glm::vec3{0, 1, 0}, 10.f);
    // auto  model = scene.addShape<Model>("model/dragon.obj");
    //
    // for (int i = -3; i <= 3; i++) {
    //     auto mat = scene.addMaterial<Dielectric>(1.f + 0.2f * (i + 3), glm::vec3{1, 1, 1},
    //                                              (3.f - i) / 18.f, (3.f - i) / 6.f);
    //     scene.addInstance(sphere, mat, {0, 0.5, i * 2}, {0.8, 0.8, 0.8});
    // }
    //
    // for (int i = -3; i <= 3; i++) {
    //     glm::vec3 c = RGB::GenerateHeatMapRGB((i + 3.f) / 6.f);
    //     auto mat = scene.addMaterial<Conductor>(glm::vec3{2.f - c * 2.f}, glm::vec3{2.f + c * 3.f},
    //                                             (3.f - i) / 6.f, (3.f - i) / 18.f);
    //     scene.addInstance(sphere, mat, {0, 2.5, i * 2}, {0.8, 0.8, 0.8});
    // }
    //
    // auto dragonMat1 = scene.addMaterial<Dielectric>(1.8, RGB{128, 211, 131}, 0.2, 0.2);
    // scene.addInstance(model, dragonMat1, {-5, 0.4, 1.5}, {2, 2, 2});
    //
    // auto dragonMat2 =
    //     scene.addMaterial<Conductor>(glm::vec3{0.1, 1.2, 1.8}, glm::vec3{5, 2.5, 2}, 0.4, 0.4);
    // scene.addInstance(model, dragonMat2, {-5, 0.4, -1.5}, {2, 2, 2});
    //
    // auto groundMat = scene.addMaterial<Ground>(RGB(120, 204, 157));
    // scene.addInstance(plane, groundMat, {0, -0.5, 0});
    //
    // auto areaLightMat = scene.addMaterial<Diffuse>();
    // auto areaLightShape = scene.addShape<Sphere>(glm::vec3{-2, 6, 0}, 0.5f);
    // scene.addAreaLight(areaLightShape, areaLightMat, glm::vec3{0.95 * 5, 0.95 * 5, 1.f * 5}, false);
    // scene.addInfiniteLight(glm::vec3{0.9, 0.9, 0.7});
    // scene.build();
    //
    // PathTracingRenderer pathTracingRenderer{camera, scene};
    // Previewer           previewer{pathTracingRenderer};
    // if (previewer.preview()) {
    //     pathTracingRenderer.render(32, "PT_with_sample_light.ppm");
    // }
    CornellBox();
}