#pragma once

#include "pch.h"

#include "Util/rng.h"

class Material {
public:
    virtual ~Material() = default;

    virtual glm::vec3 sampleBRDF(const glm::vec3& wi, glm::vec3& beta, const RNG& rng) = 0;

    void setEmission(const glm::vec3& emission)
    {
        emissive = emission;
    }

public:
    glm::vec3 emissive{0, 0, 0};
};
