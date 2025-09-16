#pragma once

#include "pch.h"

#include "shape.h"
#include "material.h"

struct ShapeInstance {
    std::shared_ptr<Shape>    shape;
    std::shared_ptr<Material> material;
    glm::mat4                 modelMatrix;
    glm::mat4                 inverseModelMatrix;
};

struct Scene final : public Shape {
public:
    [[nodiscard]] std::optional<HitInfo> intersect(const Ray& ray, float tMin = Epsilon,
                                                   float tMax = Infinity) const override;

    void addShape(std::shared_ptr<Shape> shape, std::shared_ptr<Material> material,
                  const glm::vec3& translation = {0, 0, 0}, const glm::vec3& scale = {1, 1, 1},
                  const glm::vec3& rotation = {0, 0, 0});

private:
    std::vector<ShapeInstance> shape_;
};
