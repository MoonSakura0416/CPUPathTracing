#pragma once

#include "pch.h"

#include "aabb.h"
#include "Shape/triangle.h"

struct BVHNode {
    void updateAABB()
    {
        aabb = AABB{};
        for (const auto& tri : triangles) {
            aabb.expand(tri.p0);
            aabb.expand(tri.p1);
            aabb.expand(tri.p2);
        }
    }

    AABB                     aabb{};
    std::vector<Triangle>    triangles;
    std::unique_ptr<BVHNode> left{nullptr};
    std::unique_ptr<BVHNode> right{nullptr};
};

class BVH : public Shape {
public:
    void build(std::vector<Triangle> triangles);

    [[nodiscard]] std::optional<HitInfo> intersect(const Ray& ray, float tMin,
                                                   float tMax) const override;

private:
    void recursiveSplit(std::unique_ptr<BVHNode>& node);

    void recursiveIntersect(const std::unique_ptr<BVHNode>& node, const Ray& ray, float tMin,
                            float tMax, std::optional<HitInfo>& closestHit) const;

private:
    std::unique_ptr<BVHNode> root_;
};
