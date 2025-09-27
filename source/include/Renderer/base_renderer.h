#pragma once

#include "pch.h"

#include "Camera/camera.h"
#include "Shape/scene.h"
#include "Util/rng.h"

#define DEFINE_RENDERER(className)                                                                 \
    class className##Renderer : public BaseRenderer {                                              \
    public:                                                                                        \
        className##Renderer(Camera& camera, const Scene& scene) : BaseRenderer(camera, scene) {}   \
                                                                                                   \
    private:                                                                                       \
        glm::vec3 renderPixel(const glm::ivec2& pixelCoord, RNG& rng) override;                    \
    };

class BaseRenderer {
    friend class Previewer;

public:
    BaseRenderer(Camera& camera, const Scene& scene) : camera_(camera), scene_(scene) {}

    virtual ~BaseRenderer() = default;

    void render(size_t spp, const std::filesystem::path& path);

private:
    [[nodiscard]] virtual glm::vec3 renderPixel(const glm::ivec2& pixelCoord, RNG& rng) = 0;

protected:
    Camera&      camera_;
    const Scene& scene_;
};
