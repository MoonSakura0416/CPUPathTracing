#include "Renderer/simple_rt_renderer.h"
#include "Util/frame.h"

glm::vec3 SimpleRTRenderer::renderPixel(const glm::ivec2& pixelCoord, RNG& rng)
{
    auto      ray = camera_.getRay(pixelCoord, {rng.uniform(), rng.uniform()});
    glm::vec3 beta = {1, 1, 1};
    glm::vec3 color = {0, 0, 0};
    while (true) {
        auto hitInfo = scene_.intersect(ray);
        if (hitInfo.has_value()) {
            color += beta * hitInfo->material->emissive;
            beta *= hitInfo->material->albedo;

            ray.origin = hitInfo->hitPos;
            glm::vec3 lightDir;
            Frame     frame{hitInfo->normal};
            if (hitInfo->material->isSpecular) {
                glm::vec3 viewDir = frame.localFromWorld(-ray.direction);
                lightDir = {-viewDir.x, viewDir.y, -viewDir.z};
                // ray.direction =
                //     glm::reflect(ray.direction, hitInfo->normal);
            } else {
                do {
                    lightDir = {rng.uniform(), rng.uniform(), rng.uniform()};
                    lightDir = lightDir * 2.f - 1.f;
                } while (glm::length(lightDir) > 1.f);
                if (lightDir.y < 0) {
                    lightDir.y = -lightDir.y;
                }
                // float     u1 = uni(rng);
                // float     u2 = uni(rng);
                // glm::vec3 wiLocal = sampleCosineHemisphere(u1, u2);
                // glm::vec3 wi = frame.worldFromLocal(wiLocal);
                // ray.direction = wi;
            }
            ray.direction = frame.worldFromLocal(lightDir);
        } else {
            break;
        }
    }
    return color;
}
