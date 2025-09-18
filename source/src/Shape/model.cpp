#include "pch.h"

#include "Shape/model.h"
#include "Util/profile.h"
#include <rapidobj/rapidobj.hpp>

Model::Model(const std::filesystem::path& path)
{
    PROFILE("Load model" + path.string())

    auto result = rapidobj::ParseFile(path, rapidobj::MaterialLibrary::Ignore());
    std::vector<Triangle> triangles;
    for (const auto& shape : result.shapes) {
        size_t indexOffset = 0;
        for (size_t numFaceVertex : shape.mesh.num_face_vertices) {
            if (numFaceVertex == 3) {
                auto      index = shape.mesh.indices[indexOffset];
                glm::vec3 pos0{result.attributes.positions[index.position_index * 3 + 0],
                               result.attributes.positions[index.position_index * 3 + 1],
                               result.attributes.positions[index.position_index * 3 + 2]};
                index = shape.mesh.indices[indexOffset + 1];
                glm::vec3 pos1{result.attributes.positions[index.position_index * 3 + 0],
                               result.attributes.positions[index.position_index * 3 + 1],
                               result.attributes.positions[index.position_index * 3 + 2]};
                index = shape.mesh.indices[indexOffset + 2];
                glm::vec3 pos2{result.attributes.positions[index.position_index * 3 + 0],
                               result.attributes.positions[index.position_index * 3 + 1],
                               result.attributes.positions[index.position_index * 3 + 2]};

                if (index.normal_index >= 0) {
                    index = shape.mesh.indices[indexOffset];
                    glm::vec3 normal0{result.attributes.normals[index.normal_index * 3 + 0],
                                      result.attributes.normals[index.normal_index * 3 + 1],
                                      result.attributes.normals[index.normal_index * 3 + 2]};
                    index = shape.mesh.indices[indexOffset + 1];
                    glm::vec3 normal1{result.attributes.normals[index.normal_index * 3 + 0],
                                      result.attributes.normals[index.normal_index * 3 + 1],
                                      result.attributes.normals[index.normal_index * 3 + 2]};
                    index = shape.mesh.indices[indexOffset + 2];
                    glm::vec3 normal2{result.attributes.normals[index.normal_index * 3 + 0],
                                      result.attributes.normals[index.normal_index * 3 + 1],
                                      result.attributes.normals[index.normal_index * 3 + 2]};
                    triangles.emplace_back(pos0, pos1, pos2, normal0, normal1, normal2);
                } else {
                    triangles.emplace_back(pos0, pos1, pos2);
                }
            }
            indexOffset += numFaceVertex;
        }
    }
    bvh_.build(std::move(triangles));
}

std::optional<HitInfo> Model::intersect(const Ray& ray, float tMin, float tMax) const
{
   return bvh_.intersect(ray, tMin, tMax);
}

