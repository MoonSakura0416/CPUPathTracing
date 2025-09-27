#pragma once

#include "Camera/camera.h"
#include "Camera/film.h"
#include "Shape/scene.h"
#include "Shape/quad.h"
#include "Shape/sphere.h"
#include "Shape/model.h"
#include "Material/diffuse.h"
#include "Material/dielectric.h"
#include "Material/conductor.h"
#include "Renderer/normal_renderer.h"
#include "Renderer/path_tracing_renderer.h"
#include "Renderer/previewer.h"

inline void CornellBox()
{
    Film   film{600, 600};
    Camera camera(film, {278.f, 278.f, -800.f}, {278.f, 278.f, 0.f}, 38.f);
    Scene  scene{};

    auto redMaterial = std::make_shared<Diffuse>(glm::vec3{0.65f, 0.05f, 0.05f});
    auto whiteMaterial = std::make_shared<Diffuse>(glm::vec3{0.73f, 0.73f, 0.73f});
    auto greenMaterial = std::make_shared<Diffuse>(glm::vec3{0.15f, 0.60f, 0.18f});

    // Lighting material: It is black itself (not reflecting light), but it will shine automatically
    auto lightMaterial = std::make_shared<Diffuse>(glm::vec3{0.0f, 0.0f, 0.0f});
    lightMaterial->setEmission({15.f / 2.f, 15.f / 2.f, 15.f / 2.f});

    // Left wall (red), x=0, normal direction +X
    auto leftWall =
        std::make_shared<Quad>(glm::vec3{0, 0, 0}, glm::vec3{0, 555, 0}, glm::vec3{0, 0, 555});
    scene.addShape(leftWall, redMaterial);

    // Right wall (green), x=555, normal direction -X
    auto rightWall =
        std::make_shared<Quad>(glm::vec3{555, 0, 0}, glm::vec3{0, 0, 555}, glm::vec3{0, 555, 0});
    scene.addShape(rightWall, greenMaterial);

    // Back wall (white), z=555, normal facing -Z
    auto backWall =
        std::make_shared<Quad>(glm::vec3{0, 0, 555}, glm::vec3{0, 555, 0}, glm::vec3{555, 0, 0});
    scene.addShape(backWall, whiteMaterial);

    // Floor (white), y=0, normal direction +Y
    auto floor =
        std::make_shared<Quad>(glm::vec3{0, 0, 0}, glm::vec3{0, 0, 555}, glm::vec3{555, 0, 0});
    scene.addShape(floor, whiteMaterial);

    // Ceiling (white), y=555, Normal facing -Y
    auto ceiling =
        std::make_shared<Quad>(glm::vec3{0, 555, 0}, glm::vec3{555, 0, 0}, glm::vec3{0, 0, 555});
    scene.addShape(ceiling, whiteMaterial);

    // Light source (on the ceiling), y=554, normal direction -Y
    auto light = std::make_shared<Quad>(glm::vec3{180.5, 554, 200.75}, glm::vec3{195, 0, 0},
                                        glm::vec3{0, 0, 157.5});
    scene.addShape(light, lightMaterial);
    auto glassMaterial = std::make_shared<Dielectric>(1.5f, glm::vec3{1, 1, 1}, 0.1f, 0.1f);

    auto sphere = std::make_shared<Sphere>(glm::vec3{0, 0, 0}, 70.f);

    scene.addShape(sphere, glassMaterial, {160, 70, 125});

    // --- Golden dragon (Conductor) ---
    glm::vec3 goldEta = {0.143f, 0.449f, 1.399f};
    glm::vec3 goldK = {3.638f, 2.474f, 1.905f};
    auto      goldMaterial = std::make_shared<Conductor>(goldEta, goldK, 0.25f, 0.25f);
    auto      model = std::make_shared<Model>("model/dragon.obj");
    scene.addShape(model, goldMaterial, {350, 100, 350}, {340, 340, 340}, {0, 90.0, 0});
    scene.build();

    PathTracingRenderer pathTracingRenderer{camera, scene};
    Previewer previewer{pathTracingRenderer};
    if (previewer.preview()) {
        pathTracingRenderer.render(1000, "cornellbox3.ppm");
    }
}
