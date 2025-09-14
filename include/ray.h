#pragma once

#include <glm/glm.hpp>

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;

    [[nodiscard]] glm::vec3 at(float t) const { return origin + t * direction; }
};
