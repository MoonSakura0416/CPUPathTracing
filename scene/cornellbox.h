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

    auto redMaterial = scene.addMaterial<Diffuse>(glm::vec3{0.65f, 0.05f, 0.05f});
    auto whiteMaterial = scene.addMaterial<Diffuse>(glm::vec3{0.73f, 0.73f, 0.73f});
    auto greenMaterial = scene.addMaterial<Diffuse>(glm::vec3{0.15f, 0.60f, 0.18f});


    // Left wall (red), x=0, normal direction +X
    auto leftWall =
        scene.addShape<Quad>(glm::vec3{0, 0, 0}, glm::vec3{0, 555, 0}, glm::vec3{0, 0, 555});
    scene.addInstance(leftWall, redMaterial);

    // Right wall (green), x=555, normal direction -X
    auto rightWall =
        scene.addShape<Quad>(glm::vec3{555, 0, 0}, glm::vec3{0, 0, 555}, glm::vec3{0, 555, 0});
    scene.addInstance(rightWall, greenMaterial);

    // Back wall (white), z=555, normal facing -Z
    auto backWall =
        scene.addShape<Quad>(glm::vec3{0, 0, 555}, glm::vec3{0, 555, 0}, glm::vec3{555, 0, 0});
    scene.addInstance(backWall, whiteMaterial);

    // Floor (white), y=0, normal direction +Y
    auto floor =
        scene.addShape<Quad>(glm::vec3{0, 0, 0}, glm::vec3{0, 0, 555}, glm::vec3{555, 0, 0});
    scene.addInstance(floor, whiteMaterial);

    // Ceiling (white), y=555, Normal facing -Y
    auto ceiling =
        scene.addShape<Quad>(glm::vec3{0, 555, 0}, glm::vec3{555, 0, 0}, glm::vec3{0, 0, 555});
    scene.addInstance(ceiling, whiteMaterial);

    // Light source (on the ceiling), y=554, normal direction -Y
    auto light = scene.addShape<Quad>(glm::vec3{180.5, 554, 200.75}, glm::vec3{195, 0, 0},
                                        glm::vec3{0, 0, 157.5});
    // Lighting material: It is black itself (not reflecting light), but it will shine automatically
    auto lightMaterial = scene.addMaterial<Diffuse>(glm::vec3{0.0f, 0.0f, 0.0f});
    scene.addAreaLight(light, lightMaterial, glm::vec3{15.f / 2.f, 15.f / 2.f, 15.f / 2.f}, false);



    auto glassMaterial = scene.addMaterial<Dielectric>(1.5f, glm::vec3{1, 1, 1}, 0.1f, 0.1f);
    auto sphere = scene.addShape<Sphere>(glm::vec3{0, 0, 0}, 70.f);
    scene.addInstance(sphere, glassMaterial, {160, 70, 125});

    // --- Golden dragon (Conductor) ---
    glm::vec3 goldEta = {0.143f, 0.449f, 1.399f};
    glm::vec3 goldK = {3.638f, 2.474f, 1.905f};
    auto      goldMaterial = scene.addMaterial<Conductor>(goldEta, goldK, 0.25f, 0.25f);
    auto      model = scene.addShape<Model>("model/dragon.obj");
    scene.addInstance(model, goldMaterial, {350, 100, 350}, {340, 340, 340}, {0, 90.0, 0});
    scene.build();

    PathTracingRenderer pathTracingRenderer{camera, scene};
    Previewer previewer{pathTracingRenderer};
    if (previewer.preview()) {
        pathTracingRenderer.render(1500, "cornellboxTest4.ppm");
    }
}
