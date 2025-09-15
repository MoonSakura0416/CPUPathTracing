#include "model.h"

#include <print>
#include <iostream>
#include <sstream>
#include <fstream>

Model::Model(const std::filesystem::path& path)
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;

    std::ifstream file(path);
    if (!file.is_open()) {
        std::println(std::cerr, "Failed to open file: {}", path.string());
        return;
    }

    std::string line;
    char        trash;
    while (!file.eof()) {
        std::getline(file, line);
        std::istringstream iss(line);

        if (line.starts_with("v ")) {
            glm::vec3 position;
            iss >> trash >> position.x >> position.y >> position.z;
            positions.push_back(position);
        } else if (line.starts_with("vn ")) {
            glm::vec3 normal;
            iss >> trash >> trash >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        } else if (line.starts_with("f ")) {
            // f 1//3 2//3 3//3
            glm::ivec3 vIdx, vnIdx;
            iss >> trash;
            iss >> vIdx.x >> trash >> trash >> vnIdx.x;
            iss >> vIdx.y >> trash >> trash >> vnIdx.y;
            iss >> vIdx.z >> trash >> trash >> vnIdx.z;
            triangles_.emplace_back(positions[vIdx.x - 1], positions[vIdx.y - 1],
                                    positions[vIdx.z - 1], normals[vnIdx.x - 1],
                                    normals[vnIdx.y - 1], normals[vnIdx.z - 1]);
        }
    }
}

std::optional<HitInfo> Model::intersect(const Ray& ray, float tMin, float tMax) const
{
    std::optional<HitInfo> closestHit{};

    for (auto& triangle : triangles_) {
        auto hitInfo = triangle.intersect(ray, tMin, tMax);
        if (hitInfo.has_value()) {
            tMax = hitInfo->hitT;
            closestHit = hitInfo;
        }
    }
    return closestHit;
}
