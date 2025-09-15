#pragma once

#include <glm/glm.hpp>

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;

    /** Computes the point along the ray at parameter t.
     */
    [[nodiscard]] glm::vec3 at(float t) const { return origin + t * direction; }
};


struct HitInfo {
    float hitT;
    glm::vec3 hitPos;
    glm::vec3 normal;

};
