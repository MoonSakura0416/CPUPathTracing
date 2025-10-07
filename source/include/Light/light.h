#pragma once

#include "pch.h"

#include "Util/rng.h"

struct LightSample {
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 emission;
    float     pdf;
};

class Light {
public:
    explicit Light(const glm::vec3& emission) : emission_(emission) {}
    virtual ~Light() = default;

    [[nodiscard]] virtual glm::vec3 getEmission(const glm::vec3& surfacePoint,
                                                const glm::vec3& lightPoint,
                                                const glm::vec3& normal) const
    {
        return emission_;
    }

    [[nodiscard]] virtual std::optional<LightSample>
    lightSample(const glm::vec3& surfacePoint, float sceneRadius, const RNG& rng) const = 0;

    [[nodiscard]] virtual float Phi(float radius) const = 0;

protected:
    glm::vec3 emission_;
};
