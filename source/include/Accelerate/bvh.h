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

    AABB                  aabb{};
    std::vector<Triangle> triangles;
    BVHTreeNode*          left{nullptr};
    BVHTreeNode*          right{nullptr};
    size_t                depth{0};
    size_t                splitAxis{0};
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

    void addLeaf(const BVHTreeNode* node)
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

class BVHNodeAllocator {
public:
    BVHNodeAllocator() : ptr_(BlockSize) {}

    BVHTreeNode* allocate() {
        if (ptr_ == BlockSize) {
            nodeBlocks_.push_back(std::make_unique<BVHTreeNode[]>(BlockSize));
            ptr_ = 0;
        }
        // Return a raw pointer to the next available node in the current block and advance the pointer.
        return &nodeBlocks_.back()[ptr_++];
    }

private:

    static constexpr size_t BlockSize = 4096;
    size_t ptr_;
    std::vector<std::unique_ptr<BVHTreeNode[]>> nodeBlocks_;
};

class BVH final : public Shape {
public:
    void build(std::vector<Triangle> triangles);

    [[nodiscard]] std::optional<HitInfo> intersect(const Ray& ray, float tMin,
                                                   float tMax) const override;

private:
    void recursiveSplit(BVHTreeNode* node, BVHState& state);

    size_t recursiveFlatten(const BVHTreeNode* node);

private:
    BVHNodeAllocator nodeAllocator_;
    std::vector<BVHNode> nodes_;
    std::vector<Triangle> triangles_;
};
