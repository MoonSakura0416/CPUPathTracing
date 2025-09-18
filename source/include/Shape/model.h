#pragma once

#include "pch.h"

#include "shape.h"
#include "triangle.h"
#include "Accelerate/bvh.h"

class Model final : public Shape{
public:
    explicit Model(const std::vector<Triangle>& triangles)
    {
        bvh_.build(triangles);
    }

    explicit Model(const std::filesystem::path& path);

    [[nodiscard]] std::optional<HitInfo> intersect(const Ray& ray, float tMin,
                                                   float tMax) const override;

private:
    BVH bvh_    ;
};
