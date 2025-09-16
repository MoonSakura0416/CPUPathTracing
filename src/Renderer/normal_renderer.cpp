#include "Renderer/normal_renderer.h"

glm::vec3 NormalRenderer::renderPixel(const glm::ivec2& pixelCoord, RNG& rng)
{
    auto ray = camera_.getRay(pixelCoord);
    auto hitInfo = scene_.intersect(ray);
    if (hitInfo.has_value()) {
        return hitInfo->normal * 0.5f + 0.5f;
    }
    return {0,0,0};
}
