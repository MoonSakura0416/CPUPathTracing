#pragma once

#include "pch.h"

#include "Shape/material.h"


struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;

    /** Computes the point along the ray at parameter t.
     */
    [[nodiscard]] glm::vec3 at(float t) const { return origin + t * direction; }

    /** Transforms the ray from world space to the model space.
     */
    [[nodiscard]] Ray transform(const glm::mat4& invModel) const;
};


struct HitInfo {
    float hitT;
    glm::vec3 hitPos;
    glm::vec3 normal;
    std::shared_ptr<Material> material;
};

