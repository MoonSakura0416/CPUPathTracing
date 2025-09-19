#include "pch.h"

#include "Shape/model.h"
#include "Util/profile.h"
#include <rapidobj/rapidobj.hpp>

Model::Model(const std::filesystem::path& path)
{
     PROFILE("Load model " + path.string())

    auto result = rapidobj::ParseFile(path, rapidobj::MaterialLibrary::Ignore());

    size_t triEstimate = 0;
    for (const auto& shape : result.shapes)
        for (size_t nfv : shape.mesh.num_face_vertices)
            if (nfv == 3) ++triEstimate;

    std::vector<Triangle> triangles;
    triangles.reserve(triEstimate);

    auto isFinite3 = [](const glm::vec3& v) {
        using std::isfinite;
        return isfinite(v.x) && isfinite(v.y) && isfinite(v.z);
    };
    auto nonZero = [](const glm::vec3& v, float eps2 = 1e-20f) {
        return glm::dot(v, v) > eps2;
    };
    auto readPos = [&](int idx) -> glm::vec3 {
        const float* p = result.attributes.positions.data() + idx * 3;
        return {p[0], p[1], p[2]};
    };
    auto readNrm = [&](int idx) -> glm::vec3 {
        const float* n = result.attributes.normals.data() + idx * 3;
        return {n[0], n[1], n[2]};
    };

    size_t fallbackNormals = 0;

    for (const auto& shape : result.shapes) {
        size_t indexOffset = 0;
        const auto& indices = shape.mesh.indices;

        for (size_t nfv : shape.mesh.num_face_vertices) {
            if (nfv == 3) {
                const auto i0 = indices[indexOffset + 0];
                const auto i1 = indices[indexOffset + 1];
                const auto i2 = indices[indexOffset + 2];

                const glm::vec3 p0 = readPos(i0.position_index);
                const glm::vec3 p1 = readPos(i1.position_index);
                const glm::vec3 p2 = readPos(i2.position_index);

                const bool haveAllNormals =
                    (i0.normal_index >= 0) && (i1.normal_index >= 0) && (i2.normal_index >= 0);

                if (haveAllNormals) {
                    glm::vec3 n0 = readNrm(i0.normal_index);
                    glm::vec3 n1 = readNrm(i1.normal_index);
                    glm::vec3 n2 = readNrm(i2.normal_index);

                    const bool normalsOk =
                        isFinite3(n0) && isFinite3(n1) && isFinite3(n2) &&
                        nonZero(n0)    && nonZero(n1)    && nonZero(n2);

                    if (normalsOk) {
                        triangles.emplace_back(p0, p1, p2, n0, n1, n2);
                    } else {
                        ++fallbackNormals;
                        triangles.emplace_back(p0, p1, p2);
                    }
                } else {
                    ++fallbackNormals;
                    triangles.emplace_back(p0, p1, p2);
                }
            }
            indexOffset += nfv;
        }
    }

    //std::println("fallbackNormals = {}", fallbackNormals);

    bvh_.build(std::move(triangles));
}

std::optional<HitInfo> Model::intersect(const Ray& ray, float tMin, float tMax) const
{
   return bvh_.intersect(ray, tMin, tMax);
}

