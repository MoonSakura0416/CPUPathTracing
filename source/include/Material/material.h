#pragma once

#include "pch.h"

#include "Util/rng.h"

struct BSDFSample {
    glm::vec3 bsdf;
    float     pdf;
    glm::vec3 lightDir;
};

class Material {
public:
    virtual ~Material() = default;

    virtual std::optional<BSDFSample> sampleBSDF(const glm::vec3& hitPos, const glm::vec3& wi,
                                                 const RNG& rng) = 0;

    void setEmission(const glm::vec3& emission)
    {
        emissive = emission;
    }

public:
    glm::vec3 emissive{0, 0, 0};
};
