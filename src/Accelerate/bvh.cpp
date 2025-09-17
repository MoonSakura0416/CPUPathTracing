#include <algorithm>

#include "Accelerate/bvh.h"

void BVH::build(std::vector<Triangle> triangles)
{
    root_ = std::make_unique<BVHNode>();
    root_->triangles = std::move(triangles);
    root_->updateAABB();
    recursiveSplit(root_);
}

void BVH::recursiveSplit(std::unique_ptr<BVHNode>& node)
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
    auto first  = std::ranges::begin(tris);
    if (midIt == first || midIt == last) {
        auto nth = tris.begin() + tris.size() / 2;
        auto cmp = [axis](const Triangle& a, const Triangle& b) {
            const float ca = (a.p0[axis] + a.p1[axis] + a.p2[axis]) * (1.0f / 3.0f);
            const float cb = (b.p0[axis] + b.p1[axis] + b.p2[axis]) * (1.0f / 3.0f);
            return ca < cb;
        };
        std::ranges::nth_element(tris, nth, cmp);
        midIt = nth;
    }

    node->left = std::make_unique<BVHNode>();
    node->right = std::make_unique<BVHNode>();

    node->left->triangles.assign(std::make_move_iterator(first),
                                 std::make_move_iterator(midIt));

    node->right->triangles.assign(std::make_move_iterator(midIt),
                                  std::make_move_iterator(last));

    node->triangles = {};

    node->left->updateAABB();
    node->right->updateAABB();

    recursiveSplit(node->left);
    recursiveSplit(node->right);
}

std::optional<HitInfo> BVH::intersect(const Ray& ray, float tMin, float tMax) const
{
    std::optional<HitInfo> closestHit;
    recursiveIntersect(root_, ray, tMin, tMax, closestHit);
    return closestHit;
}

void BVH::recursiveIntersect(const std::unique_ptr<BVHNode>& node, const Ray& ray, float tMin,
                             float tMax, std::optional<HitInfo>& closestHit) const
{
    if (!node)
        return;

    if (!node->aabb.hasIntersection(ray, tMin, tMax)) {
        return;
    }

    const bool isLeaf = (!node->left && !node->right);
    if (isLeaf) {
        for (const auto& triangle : node->triangles) {
            auto hitInfo = triangle.intersect(ray, tMin, tMax);
            if (hitInfo.has_value()) {
                tMax = hitInfo->hitT;
                closestHit = hitInfo;
            }
        }
    } else {
        recursiveIntersect(node->left, ray, tMin, tMax, closestHit);
        const float newTMax = closestHit ? closestHit->hitT : tMax;
        recursiveIntersect(node->right, ray, tMin, newTMax, closestHit);
    }
}
