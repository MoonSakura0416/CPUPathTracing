#pragma once

#include <glm/glm.hpp>

class Frame {
public:
    explicit Frame(const glm::vec3& normal);

    [[nodiscard]] glm::vec3 localFromWorld(const glm::vec3& world) const;
    [[nodiscard]] glm::vec3 worldFromLocal(const glm::vec3& local) const;

private:
    glm::vec3 xAxis_, yAxis_, zAxis_;
};