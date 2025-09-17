#pragma once

#include "pch.h"

#include "aabb.h"
#include "Shape/triangle.h"

struct BVHTreeNode {
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
    std::unique_ptr<BVHTreeNode> left{nullptr};
    std::unique_ptr<BVHTreeNode> right{nullptr};
};

struct alignas(32) BVHNode {
    AABB aabb{};
    union {
        int childIndex; // index of right child, 0 means leaf node
        int triStart;   // start index of triangles in leaf node
    };
    int triCount{0};   // number of triangles in leaf node
};

class BVH final : public Shape {
public:
    void build(std::vector<Triangle> triangles);

    [[nodiscard]] std::optional<HitInfo> intersect(const Ray& ray, float tMin,
                                                   float tMax) const override;

private:
    void recursiveSplit(std::unique_ptr<BVHTreeNode>& node);

    size_t recursiveFlatten(const std::unique_ptr<BVHTreeNode>& node);

private:
    std::vector<BVHNode> nodes_;
    std::vector<Triangle> triangles_;
};
