#include "Renderer/debug_renderer.h"
#include "Util/rgb.h"

glm::vec3 AABBTestCountRenderer::renderPixel(const glm::ivec2& pixelCoord, RNG& rng)
{
#ifdef WITH_DEBUG_INFO
    auto ray = camera_.getRay(pixelCoord);
    auto hitInfo = scene_.intersect(ray);
    if (hitInfo.has_value()) {
        return RGB::GenerateHeatMapRGB(hitInfo->aabbTestCount / 200.f);
    }
    return {};
#else
#endif
}

glm::vec3 TriTestCountRenderer::renderPixel(const glm::ivec2& pixelCoord, RNG& rng)
{
#ifdef WITH_DEBUG_INFO
    auto ray = camera_.getRay(pixelCoord);
    auto hitInfo = scene_.intersect(ray);
    if (hitInfo.has_value()) {
        return RGB::GenerateHeatMapRGB(hitInfo->triTestCount / 20.f);
    }
    return {};
#else
#endif
}

glm::vec3 AABBTestDepthRenderer::renderPixel(const glm::ivec2& pixelCoord, RNG& rng)
{
#ifdef WITH_DEBUG_INFO
    auto ray = camera_.getRay(pixelCoord);
    auto hitInfo = scene_.intersect(ray);
    if (hitInfo.has_value()) {
        return RGB::GenerateHeatMapRGB(hitInfo->aabbTestDepth / 64.f);
    }
    return {};
#else
#endif
}
