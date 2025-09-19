#include "Accelerate/bvh_scene.h"
#include "Util/debug_macro.h"

void SceneBVH::build(std::vector<ShapeInstance> instances)
{
    const auto root = nodeAllocator_.allocate();
    for (auto& instance : instances) {
        if (instance.shape->getBound().isValid()) {
            instance.updateAABB();
            root->instances.push_back(instance);
        } else {
            infinityInstances_.push_back(instance);
        }
    }
    root->depth = 1;
    root->updateAABB();
    SceneBVHState state{};
    size_t        totalTriCount = root->instances.size();
    recursiveSplit(root, state);

    constexpr int labelWidth = 22;
    std::println("{0:<{2}} {1}", "Total Node:", state.totalNodeCount, labelWidth);
    std::println("{0:<{2}} {1}", "Leaf Node:", state.leafCount, labelWidth);
    std::println("{0:<{2}} {1}", "Total ShapeInstances:", totalTriCount, labelWidth);
    std::println("{0:<{2}} {1}", "Max Leaf ShapeInstances:", state.maxLeafTriCount, labelWidth);
    std::println("{0:<{2}} {1:.2f}",
                 "Mean Leaf ShapeInstances:", static_cast<float>(totalTriCount) / state.leafCount,
                 labelWidth);
    std::println("{0:<{2}} {1}", "Max Depth:", state.maxDepth, labelWidth);

    nodes_.reserve(state.totalNodeCount);
    instances_.reserve(totalTriCount);
    recursiveFlatten(root);
}

void SceneBVH::recursiveSplit(SceneBVHTreeNode* node, SceneBVHState& state)
{
    state.totalNodeCount++;
    size_t totalTriCount = node->instances.size();
    if (totalTriCount <= 4 || node->depth >= 64) {
        state.addLeaf(node);
        return;
    }

    // --- SAH start of segmentation logic ---
    constexpr int   numBins = 12;
    constexpr float traversalCost = 0.125f;  // The cost of traversing nodes
    constexpr float intersectCost = 1.0f;    // the cost of handling instanceangle intersection

    float minCost = Infinity;
    // AABB minLeftAABB{}, minRightAABB{};
    size_t bestAxis = -1;
    float  bestSplitPos = 0.0f;

    const float parentSurfaceArea = node->aabb.surfaceArea();
    const float noSplitCost = intersectCost * totalTriCount;

    for (size_t axis = 0; axis < 3; ++axis) {
        const float axisRange = node->aabb.diagonal()[axis];
        if (axisRange < 1e-6f)
            continue;

        struct Bin {
            AABB aabb{};
            int  instanceCount{0};
        };
        std::array<Bin, numBins> bins;

        for (const auto& instance : node->instances) {
            const float c = instance.center[axis];
            int binIdx = static_cast<int>(numBins * ((c - node->aabb.min[axis]) / axisRange));
            binIdx = std::clamp(binIdx, 0, numBins - 1);

            bins[binIdx].instanceCount++;
            bins[binIdx].aabb.expand(instance.aabb);
        }

        // Evaluate each possible split plane (between boxes）
        std::array<float, numBins - 1> rightCost{};
        std::array<AABB, numBins - 1>  rightAABBs{};
        AABB                           cumulativeRightAABB{};
        int                            rightTriCount = 0;
        for (int i = numBins - 1; i > 0; --i) {
            rightTriCount += bins[i].instanceCount;
            cumulativeRightAABB.expand(bins[i].aabb);
            rightCost[i - 1] = cumulativeRightAABB.surfaceArea() * rightTriCount;
            rightAABBs[i - 1] = cumulativeRightAABB;
        }

        AABB leftAABB{};
        int  leftTriCount = 0;
        for (int i = 0; i < numBins - 1; ++i) {
            leftTriCount += bins[i].instanceCount;
            leftAABB.expand(bins[i].aabb);

            if (leftTriCount == 0 || leftTriCount == totalTriCount)
                continue;

            const float currentRightCost = rightCost[i];
            // const AABB& currentRightAABB = rightAABBs[i];

            float cost =
                traversalCost +
                (leftAABB.surfaceArea() * leftTriCount + currentRightCost) / parentSurfaceArea;

            if (cost < minCost) {
                minCost = cost;
                bestAxis = axis;
                bestSplitPos = node->aabb.min[axis] + (i + 1) * (axisRange / numBins);
                // minLeftAABB = leftAABB;
                // minRightAABB = currentRightAABB;
            }
        }
    }

    // If no good split found, make this a leaf node
    if (minCost >= noSplitCost) {
        state.addLeaf(node);
        return;
    }

    // --- SAH end of segmentation logic ---

    // Use the best split axes and positions found to divide the instanceangle
    auto& instances = node->instances;
    auto  pred = [bestAxis, bestSplitPos](const ShapeInstance& t) {
        const float c = t.center[bestAxis];
        return c < bestSplitPos;
    };

    auto [midIt, last] = std::ranges::partition(instances, pred);
    auto first = std::ranges::begin(instances);

    // Alternative solution (when the SAH selected segmentation point causes all instances to be on
    // one side)
    bool fallback = (midIt == first || midIt == last);
    if (fallback) {
        // Return to equal segmentation
        auto nth = instances.begin() + instances.size() / 2;
        auto cmp = [bestAxis](const ShapeInstance& a, const ShapeInstance& b) {
            const float ca = a.center[bestAxis];
            const float cb = b.center[bestAxis];
            return ca < cb;
        };
        std::ranges::nth_element(instances, nth, cmp);
        midIt = nth;
        last = std::ranges::end(instances);
    }

    node->splitAxis = bestAxis;

    node->left = nodeAllocator_.allocate();
    node->right = nodeAllocator_.allocate();

    node->left->instances.assign(std::make_move_iterator(first), std::make_move_iterator(midIt));
    node->left->depth = node->depth + 1;
    node->right->instances.assign(std::make_move_iterator(midIt), std::make_move_iterator(last));
    node->right->depth = node->depth + 1;
    node->instances = {};

    node->left->updateAABB();
    node->right->updateAABB();

    recursiveSplit(node->left, state);
    recursiveSplit(node->right, state);
}

size_t SceneBVH::recursiveFlatten(const SceneBVHTreeNode* node)
{
    const SceneBVHNode SceneBVHNode{node->aabb, 0, static_cast<uint16_t>(node->instances.size()),
                                    static_cast<uint8_t>(node->splitAxis)};
    const auto         idx = nodes_.size();  // Current node index
    nodes_.push_back(SceneBVHNode);

    if (SceneBVHNode.instanceCount == 0) {
        recursiveFlatten(node->left);
        nodes_[idx].childIndex = recursiveFlatten(node->right);
    } else {
        nodes_[idx].instanceStart = instances_.size();
        instances_.insert(instances_.end(), node->instances.begin(), node->instances.end());
    }
    return idx;
}

std::optional<HitInfo> SceneBVH::intersect(const Ray& ray, float tMin, float tMax) const
{
    std::optional<HitInfo> closestHit;
    const ShapeInstance*   closestInstance{nullptr};

    DEBUG_LINE(size_t aabbTestCount = 0)

    glm::bvec3 dirIsNeg{ray.direction.x < 0, ray.direction.y < 0, ray.direction.z < 0};

    std::array<int, 64> stack{};
    auto                ptr = stack.begin();
    size_t              currentIndex = 0;
    while (true) {
        auto& node = nodes_[currentIndex];
        DEBUG_LINE(++aabbTestCount)
        if (!node.aabb.hasIntersection(ray, tMin, tMax)) {
            if (ptr == stack.begin()) {
                break;
            }
            currentIndex = *(--ptr);
            continue;
        }

        if (node.instanceCount == 0) {  // internal node
            if (dirIsNeg[node.splitAxis]) {
                *(ptr++) = currentIndex + 1;
                currentIndex = node.childIndex;
            } else {
                currentIndex++;
                *(ptr++) = node.childIndex;
            }
        } else {  // leaf node
            auto instanceIter = instances_.begin() + node.instanceStart;
            for (size_t i = 0; i < node.instanceCount; i++) {
                Ray  localRay = ray.transform(instanceIter->inverseModelMatrix);
                auto hitInfo = instanceIter->shape->intersect(localRay, tMin, tMax);
                DEBUG_LINE(ray.aabbTestCount += localRay.aabbTestCount)
                DEBUG_LINE(ray.triTestCount += localRay.triTestCount)
                if (hitInfo.has_value()) {
                    tMax = hitInfo->hitT;
                    closestHit = hitInfo;
                    closestInstance = &(*instanceIter);
                }
                ++instanceIter;
            }
            if (ptr == stack.begin()) {
                break;
            }
            currentIndex = *(--ptr);
        }
    }

    for (const auto& infinityInstance : infinityInstances_) {
        Ray  localRay = ray.transform(infinityInstance.inverseModelMatrix);
        auto hitInfo = infinityInstance.shape->intersect(localRay, tMin, tMax);
        DEBUG_LINE(ray.aabbTestCount += localRay.aabbTestCount)
        DEBUG_LINE(ray.triTestCount += localRay.triTestCount)
        if (hitInfo.has_value()) {
            tMax = hitInfo->hitT;
            closestHit = hitInfo;
            closestInstance = &infinityInstance;
        }
    }

    if (closestInstance) {
        closestHit->hitPos = closestInstance->modelMatrix * glm::vec4(closestHit->hitPos, 1.0f);
        closestHit->normal =
            glm::normalize(glm::vec3{glm::transpose(closestInstance->inverseModelMatrix) *
                                     glm::vec4(closestHit->normal, 0.0f)});
        closestHit->material = closestInstance->material;
    }

    DEBUG_LINE(ray.aabbTestCount = aabbTestCount)
    return closestHit;
}
