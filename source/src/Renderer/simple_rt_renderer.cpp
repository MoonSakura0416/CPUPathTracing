#include "Renderer/simple_rt_renderer.h"
#include "Util/frame.h"

// #define GLM_ENABLE_EXPERIMENTAL
// #include <glm/gtx/component_wise.hpp>

inline glm::vec3 sampleCosineHemisphere(float u1, float u2) {
    const float r  = std::sqrt(u1);
    const float th = 2.f * std::numbers::pi_v<float> * u2;
    const float x = r * std::cos(th);
    const float z = r * std::sin(th);
    const float y = std::sqrt(std::max(0.f, 1.f - x*x - z*z));
    return glm::normalize(glm::vec3(x, y, z)); // 单位化
}

glm::vec3 SimpleRTRenderer::renderPixel(const glm::ivec2& pixelCoord, RNG& rng)
{
    auto      ray = camera_.getRay(pixelCoord, {rng.uniform(), rng.uniform()});
    glm::vec3 beta = {1, 1, 1};
    glm::vec3 color = {0, 0, 0};
    size_t maxBounces = 32;
    while (maxBounces--) {
        // if (glm::any(glm::isnan(ray.origin)) || glm::any(glm::isinf(ray.origin)) ||
        //     glm::any(glm::isnan(ray.direction)) || glm::any(glm::isinf(ray.direction))) {
        //     break;
        // }
        auto hitInfo = scene_.intersect(ray);
        if (hitInfo.has_value()) {
            color += beta * hitInfo->material->emissive;
            beta *= hitInfo->material->albedo;
            ray.origin    = hitInfo->hitPos + 1e-4f * hitInfo->normal;
            glm::vec3 lightDir;
            Frame     frame{hitInfo->normal};
            if (hitInfo->material->isSpecular) {
                // glm::vec3 viewDir = frame.localFromWorld(-ray.direction);
                // lightDir = {-viewDir.x, viewDir.y, -viewDir.z};
                lightDir = glm::reflect(ray.direction, hitInfo->normal);
            } else {
                do {
                    lightDir = {rng.uniform(), rng.uniform(), rng.uniform()};
                    lightDir = lightDir * 2.f - 1.f;
                } while (glm::length(lightDir) > 1.f);
                if (lightDir.y < 0) {
                    lightDir.y = -lightDir.y;
                }
                lightDir = frame.worldFromLocal(lightDir);
            }
            lightDir = glm::normalize(lightDir);
            ray.direction = lightDir;
        } else {
            break;
        }
    }
    return color;
}
