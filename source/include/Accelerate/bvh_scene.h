#pragma once

#include "pch.h"

#include "aabb.h"
#include "Shape/shape.h"

struct ShapeInstance {
    void updateAABB()
    {
        aabb = shape->getBound();
        // Transform the 8 corners of the AABB and compute the new AABB in world space
        AABB transformedAABB{};
        for (size_t i = 0; i < 8; ++i) {
            glm::vec3 corner = aabb.getCorner(i);
            glm::vec3 transformedCorner = glm::vec3(modelMatrix * glm::vec4(corner, 1.f));
            transformedAABB.expand(transformedCorner);
        }
        aabb = transformedAABB;
        center = (aabb.min + aabb.max) * 0.5f;
    }

    const Shape*    shape{nullptr};
    const Material* material{nullptr};
    glm::mat4       modelMatrix;
    glm::mat4       inverseModelMatrix;
    AABB            aabb{};
    glm::vec3       center{};
};

struct SceneBVHTreeNode {
    void updateAABB()
    {
        aabb = AABB{};
        for (const auto& instance : instances) {
            aabb.expand(instance.aabb);
        }
    }

    AABB                       aabb{};
    std::vector<ShapeInstance> instances;
    SceneBVHTreeNode*          left{nullptr};
    SceneBVHTreeNode*          right{nullptr};
    size_t                     depth{0};
    size_t                     splitAxis{0};
};

struct alignas(32) SceneBVHNode {
    AABB aabb{};
    union {
        int childIndex;     // index of right child, 0 means leaf node
        int instanceStart;  // start index of ShapeInstances in leaf node
    };
    uint16_t instanceCount{0};  // number of ShapeInstances in leaf node
    uint8_t  splitAxis{0};      // 0:x, 1:y, 2:z
};

struct SceneBVHState {
    void addLeaf(const SceneBVHTreeNode* node)
    {
        leafCount++;
        maxLeafTriCount = glm::max(maxLeafTriCount, node->instances.size());
        maxDepth = glm::max(maxDepth, node->depth);
    }

    size_t totalNodeCount{0};
    size_t leafCount{0};
    size_t maxLeafTriCount{0};
    size_t maxDepth{0};
};

class SceneBVHNodeAllocator {
public:
    SceneBVHNodeAllocator() : ptr_(BlockSize) {}

    SceneBVHTreeNode* allocate()
    {
        if (ptr_ == BlockSize) {
            nodeBlocks_.push_back(std::make_unique<SceneBVHTreeNode[]>(BlockSize));
            ptr_ = 0;
        }
        // Return a raw pointer to the next available node in the current block and advance the
        // pointer.
        return &nodeBlocks_.back()[ptr_++];
    }

private:
    static constexpr size_t                          BlockSize = 4096;
    size_t                                           ptr_;
    std::vector<std::unique_ptr<SceneBVHTreeNode[]>> nodeBlocks_;
};

class SceneBVH final : public Shape {
public:
    void build(std::vector<ShapeInstance> instances);

    [[nodiscard]] std::optional<HitInfo> intersect(const Ray& ray, float tMin,
                                                   float tMax) const override;

    [[nodiscard]] AABB getBound() const override
    {
        return nodes_[0].aabb;
    }

private:
    void recursiveSplit(SceneBVHTreeNode* node, SceneBVHState& state);

    size_t recursiveFlatten(const SceneBVHTreeNode* node);

private:
    SceneBVHNodeAllocator      nodeAllocator_;
    std::vector<SceneBVHNode>  nodes_;
    std::vector<ShapeInstance> instances_;
    std::vector<ShapeInstance> infinityInstances_;  // Shapes with infinite bounds (e.g., planes)
};
