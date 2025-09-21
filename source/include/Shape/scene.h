#pragma once

#include "pch.h"

#include "shape.h"
#include "Material/material.h"
#include "Accelerate/bvh_scene.h"

class Scene final : public Shape {
public:
    [[nodiscard]] std::optional<HitInfo> intersect(const Ray& ray, float tMin = Epsilon,
                                                   float tMax = Infinity) const override;

    void addShape(std::shared_ptr<Shape> shape, std::shared_ptr<Material> material,
                  const glm::vec3& translation = {0, 0, 0}, const glm::vec3& scale = {1, 1, 1},
                  const glm::vec3& rotation = {0, 0, 0});

    void build()
    {
        bvh_.build(std::move(shape_));
    }

private:
    std::vector<ShapeInstance> shape_;
    SceneBVH bvh_;
};
