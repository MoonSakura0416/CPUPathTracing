#include "Accelerate/bvh.h"
#include "Util/debug_macro.h"

void BVH::build(std::vector<Triangle> triangles)
{
    auto root = nodeAllocator_.allocate();
    root->triangles = std::move(triangles);
    root->depth = 1;
    root->updateAABB();
    BVHState state{};
    size_t   totalTriCount = root->triangles.size();
    recursiveSplit(root, state);

    constexpr int labelWidth = 22;
    std::println("{0:<{2}} {1}", "Total Node:", state.totalNodeCount, labelWidth);
    std::println("{0:<{2}} {1}", "Leaf Node:", state.leafCount, labelWidth);
    std::println("{0:<{2}} {1}", "Total Triangles:", totalTriCount, labelWidth);
    std::println("{0:<{2}} {1}", "Max Leaf Triangles:", state.maxLeafTriCount, labelWidth);
    std::println("{0:<{2}} {1:.2f}",
                 "Mean Leaf Triangles:", static_cast<float>(totalTriCount) / state.leafCount,
                 labelWidth);
    std::println("{0:<{2}} {1}", "Max Depth:", state.maxDepth, labelWidth);

    nodes_.reserve(state.totalNodeCount);
    triangles_.reserve(totalTriCount);
    recursiveFlatten(root);
}

void BVH::recursiveSplit(BVHTreeNode* node, BVHState& state)
{
    state.totalNodeCount++;
    size_t totalTriCount = node->triangles.size();
    if (totalTriCount <= 4 || node->depth >= 64) {
        state.addLeaf(node);
        return;
    }

    // --- SAH start of segmentation logic ---
    constexpr int numBins = 12;
    constexpr float traversalCost = 0.125f; // The cost of traversing nodes
    constexpr float intersectCost = 1.0f;   // the cost of handling triangle intersection

    float minCost = Infinity;
    AABB minLeftAABB{}, minRightAABB{};
    size_t bestAxis = -1;
    float bestSplitPos = 0.0f;

    const float parentSurfaceArea = node->aabb.surfaceArea();
    const float noSplitCost = intersectCost * totalTriCount;

    for (size_t axis = 0; axis < 3; ++axis) {
        const float axisRange = node->aabb.diagonal()[axis];
        if (axisRange < 1e-6f) continue;

        struct Bin {
            AABB aabb{};
            int triCount {0};
        };
        std::array<Bin, numBins> bins;

        for (const auto& tri : node->triangles) {
            const float c = (tri.p0[axis] + tri.p1[axis] + tri.p2[axis]) * (1.0f / 3.0f);
            int binIdx = static_cast<int>(numBins * ((c - node->aabb.min[axis]) / axisRange));
            binIdx = std::clamp(binIdx, 0, numBins - 1);

            bins[binIdx].triCount++;
            bins[binIdx].aabb.expand(tri.p0);
            bins[binIdx].aabb.expand(tri.p1);
            bins[binIdx].aabb.expand(tri.p2);
        }

        // Evaluate each possible split plane (between boxes）
        std::array<float, numBins - 1> rightCost{};
        std::array<AABB, numBins - 1> rightAABBs{};
        AABB cumulativeRightAABB{};
        int rightTriCount = 0;
        for (int i = numBins - 1; i > 0; --i) {
            rightTriCount += bins[i].triCount;
            cumulativeRightAABB.expand(bins[i].aabb);
            rightCost[i - 1] = cumulativeRightAABB.surfaceArea() * rightTriCount;
            rightAABBs[i - 1] = cumulativeRightAABB;
        }

        AABB leftAABB{};
        int leftTriCount = 0;
        for (int i = 0; i < numBins - 1; ++i) {
            leftTriCount += bins[i].triCount;
            leftAABB.expand(bins[i].aabb);

            if (leftTriCount == 0 || leftTriCount == totalTriCount) continue;

            const float currentRightCost = rightCost[i];
            const AABB& currentRightAABB = rightAABBs[i];

            float cost = traversalCost + (leftAABB.surfaceArea() * leftTriCount + currentRightCost) / parentSurfaceArea;

            if (cost < minCost) {
                minCost = cost;
                bestAxis = axis;
                bestSplitPos = node->aabb.min[axis] + (i + 1) * (axisRange / numBins);
                minLeftAABB = leftAABB;
                minRightAABB = currentRightAABB;
            }
        }
    }

    // If no good split found, make this a leaf node
    if (minCost >= noSplitCost) {
        state.addLeaf(node);
        return;
    }

    // --- SAH end of segmentation logic ---

    // Use the best split axes and positions found to divide the triangle
    auto& tris = node->triangles;
    auto pred = [bestAxis, bestSplitPos](const Triangle& t) {
        const float c = (t.p0[bestAxis] + t.p1[bestAxis] + t.p2[bestAxis]) * (1.0f / 3.0f);
        return c < bestSplitPos;
    };

    auto [midIt, last] = std::ranges::partition(tris, pred);
    auto first = std::ranges::begin(tris);

    // Alternative solution (when the SAH selected segmentation point causes all triangles to be on one side)
    bool fallback = (midIt == first || midIt == last);
    if (fallback) {
        // Return to equal segmentation
        auto nth = tris.begin() + tris.size() / 2;
        auto cmp = [bestAxis](const Triangle& a, const Triangle& b) {
            const float ca = (a.p0[bestAxis] + a.p1[bestAxis] + a.p2[bestAxis]) * (1.0f / 3.0f);
            const float cb = (b.p0[bestAxis] + b.p1[bestAxis] + b.p2[bestAxis]) * (1.0f / 3.0f);
            return ca < cb;
        };
        std::ranges::nth_element(tris, nth, cmp);
        midIt = nth;
        last = std::ranges::end(tris);
    }


    node->splitAxis = bestAxis;

    node->left = nodeAllocator_.allocate();
    node->right = nodeAllocator_.allocate();

    node->left->triangles.assign(std::make_move_iterator(first), std::make_move_iterator(midIt));
    node->left->depth = node->depth + 1;
    node->right->triangles.assign(std::make_move_iterator(midIt), std::make_move_iterator(last));
    node->right->depth = node->depth + 1;
    node->triangles = {};

    node->left->updateAABB();
    node->right->updateAABB();

    recursiveSplit(node->left, state);
    recursiveSplit(node->right, state);
}

size_t BVH::recursiveFlatten(const BVHTreeNode* node)
{
    const BVHNode bvhNode{node->aabb, 0, static_cast<uint16_t>(node->triangles.size()),
                          static_cast<uint8_t>(node->depth), static_cast<uint8_t>(node->splitAxis)};
    const auto    idx = nodes_.size();  // Current node index
    nodes_.push_back(bvhNode);

    if (bvhNode.triCount == 0) {
        recursiveFlatten(node->left);
        nodes_[idx].childIndex = recursiveFlatten(node->right);
    } else {
        nodes_[idx].triStart = triangles_.size();
        triangles_.insert(triangles_.end(), node->triangles.begin(), node->triangles.end());
    }
    return idx;
}

std::optional<HitInfo> BVH::intersect(const Ray& ray, float tMin, float tMax) const
{
    std::optional<HitInfo> closestHit;

    DEBUG_LINE(size_t aabbTestCount = 0, triTestCount = 0)

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

        if (node.triCount == 0) {  // internal node
            if (dirIsNeg[node.splitAxis]) {
                *(ptr++) = currentIndex + 1;
                currentIndex = node.childIndex;
            } else {
                currentIndex++;
                *(ptr++) = node.childIndex;
            }
        } else {  // leaf node
            auto triIter = triangles_.begin() + node.triStart;
            DEBUG_LINE(triTestCount += node.triCount)
            for (size_t i = 0; i < node.triCount; i++) {
                auto hitInfo = triIter->intersect(ray, tMin, tMax);
                ++triIter;
                if (hitInfo.has_value()) {
                    tMax = hitInfo->hitT;
                    closestHit = hitInfo;
                    DEBUG_LINE(closestHit->aabbTestDepth = node.depth)
                }
            }
            if (ptr == stack.begin()) {
                break;
            }
            currentIndex = *(--ptr);
        }
    }
    if (closestHit.has_value()) {
        DEBUG_LINE(closestHit->aabbTestCount = aabbTestCount)
        DEBUG_LINE(closestHit->triTestCount = triTestCount)
    }
    return closestHit;
}
