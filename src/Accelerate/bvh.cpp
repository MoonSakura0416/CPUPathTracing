#include <algorithm>

#include "Accelerate/bvh.h"

void BVH::build(std::vector<Triangle> triangles)
{
    auto root = std::make_unique<BVHTreeNode>();
    root->triangles = std::move(triangles);
    root->updateAABB();
    recursiveSplit(root);
    recursiveFlatten(root);
}

void BVH::recursiveSplit(std::unique_ptr<BVHTreeNode>& node)
{
    if (node->triangles.size() <= 4) {
        return;
    }

    const auto  diag = node->aabb.diagonal();
    const int   axis = (diag.x >= diag.y && diag.x >= diag.z) ? 0 : (diag.y >= diag.z) ? 1 : 2;
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

    node->right->triangles.assign(std::make_move_iterator(midIt), std::make_move_iterator(last));

    node->triangles = {};

    node->left->updateAABB();
    node->right->updateAABB();

    recursiveSplit(node->left);
    recursiveSplit(node->right);
}

size_t BVH::recursiveFlatten(const std::unique_ptr<BVHTreeNode>& node)
{
    const BVHNode bvhNode{node->aabb, 0, static_cast<int>(node->triangles.size())};
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

    std::array<int, 64> stack{};
    auto                ptr = stack.begin();
    size_t              currentIndex = 0;
    while (true) {
        auto& node = nodes_[currentIndex];

        if (!node.aabb.hasIntersection(ray, tMin, tMax)) {
            if (ptr == stack.begin()) {
                break;
            }
            currentIndex = *(--ptr);
            continue;
        }

        if (node.triCount == 0) {
            currentIndex++;
            *(ptr++) = node.childIndex;
        } else {
            auto triIter = triangles_.begin() + node.triStart;
            for (size_t i = 0; i < node.triCount; i++) {
                auto hitInfo = triIter->intersect(ray, tMin, tMax);
                ++triIter;
                if (hitInfo.has_value()) {
                    tMax = hitInfo->hitT;
                    closestHit = hitInfo;
                }
            }
            if (ptr == stack.begin()) {
                break;
            }
            currentIndex = *(--ptr);
        }
    }
    return closestHit;
}
