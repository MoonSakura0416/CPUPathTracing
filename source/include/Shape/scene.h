#pragma once

#include "pch.h"

#include "shape.h"
#include "Material/material.h"
#include "Accelerate/bvh_scene.h"
#include "Light/area_light.h"
#include "Light/infinite_light.h"
#include "Light/light_sampler.h"

class Scene final : public Shape {
public:
    [[nodiscard]] std::optional<HitInfo> intersect(const Ray& ray, float tMin = Epsilon,
                                                   float tMax = Infinity) const override;

    void addInstance(const Shape* shape, Material* material,
              const glm::vec3& translation = {0,0,0},
              const glm::vec3& scale       = {1,1,1},
              const glm::vec3& rotationDeg = {0,0,0});

    template<typename T, typename... Args>
    requires std::derived_from<T, Shape> && std::constructible_from<T, Args...>
    T* addShape(Args&&... args) {
        auto shape = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = shape.get();
        shapes_.push_back(std::move(shape));
        return ptr;
    }

    template<typename T, typename... Args>
    requires std::derived_from<T, Material> && std::constructible_from<T, Args...>
    T* addMaterial(Args&&... args) {
        auto material = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = material.get();
        materials_.push_back(std::move(material));
        return ptr;
    }

    template<typename... Args>
    void addAreaLight(const Shape* shape, Material* material, Args... args)
    {
        auto areaLight = std::make_unique<AreaLight>(shape, std::forward<Args>(args)...);
        const AreaLight* lightPtr = areaLight.get();
        assert(material);
        material->areaLight = lightPtr;
        addInstance(lightPtr->getShape(), material);
        lightSampler_.addLight(*lightPtr);
        lights_.push_back(std::move(areaLight));
    }

    template<typename... Args>
    requires std::constructible_from<InfiniteLight, Args...>
    void addInfiniteLight(Args... args)
    {
        auto infLight = std::make_unique<InfiniteLight>(std::forward<Args>(args)...);
        const InfiniteLight* lightPtr = infLight.get();
        lightSampler_.addLight(*lightPtr);
        infiniteLights_.push_back(std::move(infLight));
    }

    void build()
    {
        bvh_.build(std::move(instances_));
        auto sceneBounds = bvh_.getBound();
        radius_ = 0.5f * glm::distance(sceneBounds.max, sceneBounds.min);
        lightSampler_.build(radius_);
    }

    [[nodiscard]] float getRadius() const
    {
        return radius_;
    }

    [[nodiscard]] const LightSampler& getLightSampler() const
    {
        return lightSampler_;
    }

    [[nodiscard]] const std::vector<std::unique_ptr<const InfiniteLight>>& getInfiniteLights() const
    {
        return infiniteLights_;
    }


private:
    std::vector<std::unique_ptr<const Shape>>         shapes_;
    std::vector<std::unique_ptr<Material>>            materials_;
    std::vector<ShapeInstance>                        instances_;
    std::vector<std::unique_ptr<const Light>>         lights_;
    std::vector<std::unique_ptr<const InfiniteLight>> infiniteLights_;
    SceneBVH                                          bvh_;
    LightSampler                                      lightSampler_;
    float                                             radius_{};
};
