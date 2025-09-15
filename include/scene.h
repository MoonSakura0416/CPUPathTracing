#pragma once

#include "shape.h"

#include <memory>
#include <vector>

struct ShapeInstance {
    std::shared_ptr<Shape> shape;
    glm::mat4              modelMatrix;
    glm::mat4              inverseModelMatrix;
};

struct  Scene final : public Shape {
public:
    [[nodiscard]] std::optional<HitInfo> intersect(const Ray& ray, float tMin = Epsilon,
                                                   float tMax = Infinity) const override;

    void addShape(std::shared_ptr<Shape> shape, const glm::vec3& translation = {0, 0, 0},
                  const glm::vec3& scale = {1, 1, 1}, const glm::vec3& rotation = {0, 0, 0});

private:
    std::vector<ShapeInstance> shape_;
};
