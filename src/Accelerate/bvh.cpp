#include "Accelerate/bvh.h"
#include "Util/debug_macro.h"

void BVH::build(std::vector<Triangle> triangles)
{
    auto root = std::make_unique<BVHTreeNode>();
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

    recursiveFlatten(root);
}

void BVH::recursiveSplit(const std::unique_ptr<BVHTreeNode>& node, BVHState& state)
{
    state.totalNodeCount++;
    if (node->triangles.size() <= 4 || node->depth >= 64) {
        state.addLeaf(node);
        return;
    }

    const auto   diag = node->aabb.diagonal();
    const size_t axis = (diag.x >= diag.y && diag.x >= diag.z) ? 0
                        : (diag.y >= diag.z)                   ? 1
                                                               : 2;  // longest axis
    node->splitAxis = axis;
    const float mid = node->aabb.min[axis] + diag[axis] * 0.5f;

    auto& tris = node->triangles;

    auto pred = [axis, mid](const Triangle& t) {
        const float c = (t.p0[axis] + t.p1[axis] + t.p2[axis]) * (1.0f / 3.0f);
        return c < mid;
    };
    auto [midIt, last] = std::ranges::partition(tris, pred);
    auto first = std::ranges::begin(tris);
    if (midIt == first || midIt == last) {
        auto nth = tris.begin() + tris.size() / 2;
        auto cmp = [axis](const Triangle& a, const Triangle& b) {
            const float ca = (a.p0[axis] + a.p1[axis] + a.p2[axis]) * (1.0f / 3.0f);
            const float cb = (b.p0[axis] + b.p1[axis] + b.p2[axis]) * (1.0f / 3.0f);
            return ca < cb;
        };
        std::ranges::nth_element(tris, nth, cmp);
        midIt = nth;
        last = std::ranges::end(tris);
    }

    node->left = std::make_unique<BVHTreeNode>();
    node->right = std::make_unique<BVHTreeNode>();

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

size_t BVH::recursiveFlatten(const std::unique_ptr<BVHTreeNode>& node)
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
