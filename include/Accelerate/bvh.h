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

    AABB                         aabb{};
    std::vector<Triangle>        triangles;
    std::unique_ptr<BVHTreeNode> left{nullptr};
    std::unique_ptr<BVHTreeNode> right{nullptr};
    size_t                       depth{0};
    size_t                       splitAxis{0};
};

struct alignas(32) BVHNode {
    AABB aabb{};
    union {
        int childIndex; // index of right child, 0 means leaf node
        int triStart;   // start index of triangles in leaf node
    };
    uint16_t triCount{0};   // number of triangles in leaf node
    uint8_t  depth{0};      // depth of the node in the tree
    uint8_t  splitAxis{0};  // 0:x, 1:y, 2:z
};

struct BVHState {

    void addLeaf(const std::unique_ptr<BVHTreeNode>& node)
    {
        leafCount ++;
        maxLeafTriCount = glm::max(maxLeafTriCount, node->triangles.size());
        maxDepth = glm::max(maxDepth, node->depth);
    }

    size_t totalNodeCount{0};
    size_t leafCount{0};
    size_t maxLeafTriCount{0};
    size_t maxDepth{0};
};

class BVH final : public Shape {
public:
    void build(std::vector<Triangle> triangles);

    [[nodiscard]] std::optional<HitInfo> intersect(const Ray& ray, float tMin,
                                                   float tMax) const override;

private:
    void recursiveSplit(const std::unique_ptr<BVHTreeNode>& node, BVHState& state);

    size_t recursiveFlatten(const std::unique_ptr<BVHTreeNode>& node);

private:
    std::vector<BVHNode> nodes_;
    std::vector<Triangle> triangles_;
};
