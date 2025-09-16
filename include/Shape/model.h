#pragma once

#include "pch.h"

#include "shape.h"
#include "triangle.h"
#include "Accelerate/aabb.h"

class Model final : public Shape{
public:
    explicit Model(const std::vector<Triangle>& triangles)
        : triangles_(triangles)
    {
        build();
    }

    explicit Model(const std::filesystem::path& path);

    [[nodiscard]] std::optional<HitInfo> intersect(const Ray& ray, float tMin,
                                                   float tMax) const override;

protected:
    void build();

private:
    AABB aabb_{};
    std::vector<Triangle> triangles_;
};
