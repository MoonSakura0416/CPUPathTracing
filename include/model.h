#pragma once

#include "shape.h"
#include "triangle.h"

#include <filesystem>
#include <vector>

class Model final : public Shape{
public:
    explicit Model(const std::vector<Triangle>& triangles)
        : triangles_(triangles) {}

    explicit Model(const std::filesystem::path& path);

    [[nodiscard]] std::optional<HitInfo> intersect(const Ray& ray, float tMin,
                                                   float tMax) const override;

private:
    std::vector<Triangle> triangles_;
};
