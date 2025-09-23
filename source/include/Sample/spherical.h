#pragma once

#include "pch.h"

#include "Util/rng.h"
#include "Util/constants.h"



inline glm::vec2 UniformSampleUnitDisk(const glm::vec2 &u) {
    float r = std::sqrt(u.x);
    float theta = 2 * Pi * u.y;
    return { r * glm::cos(theta), r * glm::sin(theta) };
}

inline glm::vec3 CosineSampleHemisphere(const glm::vec2 &u) {
    float r = std::sqrt(u.x);
    float phi = 2 * Pi * u.y;
    return { r * glm::cos(phi), glm::sqrt(1 - r * r), r * glm::sin(phi) };
}

inline float CosineSampleHemispherePDF(const glm::vec3 &v) {
    return v.y > 0.0f ? (v.y / Pi) : 0.0f;
}

inline glm::vec3 UniformSampleHemisphere(const RNG &rng) {
    glm::vec3 result;
    do {
        result = { rng.uniform(), rng.uniform(), rng.uniform() };
        result = result * 2.f - 1.f;
    } while(glm::length(result) > 1);
    if (result.y < 0) {
        result.y = -result.y;
    }
    return glm::normalize(result);
}
