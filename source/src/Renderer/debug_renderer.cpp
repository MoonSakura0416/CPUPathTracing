#include "Renderer/debug_renderer.h"
#include "Util/rgb.h"

glm::vec3 AABBTestCountRenderer::renderPixel(const glm::ivec2& pixelCoord, RNG& rng)
{
#ifdef WITH_DEBUG_INFO
    auto ray = camera_.getRay(pixelCoord);
    scene_.intersect(ray);

    return RGB::GenerateHeatMapRGB(ray.aabbTestCount / 150.f);

#else
#endif
}

glm::vec3 TriTestCountRenderer::renderPixel(const glm::ivec2& pixelCoord, RNG& rng)
{
#ifdef WITH_DEBUG_INFO
    auto ray = camera_.getRay(pixelCoord);
    scene_.intersect(ray);

    return RGB::GenerateHeatMapRGB(ray.triTestCount / 12.f);

#else
#endif
}
