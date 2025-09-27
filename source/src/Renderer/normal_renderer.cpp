#include "Renderer/normal_renderer.h"
#include "Util/rgb.h"

glm::vec3 NormalRenderer::renderPixel(const glm::ivec2& pixelCoord, RNG& rng)
{
    const auto ray = camera_.getRay(pixelCoord);
    auto hitInfo = scene_.intersect(ray);
    if (hitInfo.has_value()) {
        const glm::ivec3 color = (hitInfo->normal * 0.5f + 0.5f) * 255.f;
        return RGB{color.r, color.g, color.b};
    }
    return {0, 0, 0};
}