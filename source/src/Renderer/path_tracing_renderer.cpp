#include "Renderer/path_tracing_renderer.h"
#include "Util/frame.h"
#include "Sample/spherical.h"

glm::vec3 PathTracingRenderer::renderPixel(const glm::ivec2& pixelCoord, RNG& rng)
{
    Ray ray = camera_.getRay(pixelCoord, {rng.uniform(), rng.uniform()});
    glm::vec3 beta(1.f, 1.f, 1.f);
    glm::vec3 L(0.f, 0.f, 0.f);
    const int rrStart = 5;

    for (int bounce = 0; ; ++bounce) {
        auto hitInfo = scene_.intersect(ray);
        if (!hitInfo) {
            // !Handle environment light here
            break;
        }

        L += beta * hitInfo->material->emissive;
        // BRDF = albedo / PI
        // PDF = cos_theta / PI
        // final_term = (albedo / PI) * cos_theta * (1 / PDF) = albedo
        beta *= hitInfo->material->albedo;

        Frame frame(normalize(hitInfo->normal));
        glm::vec3 wiWorld;

        if (hitInfo->material->isSpecular) {
            wiWorld = glm::reflect(ray.direction, hitInfo->normal);
        } else {
            glm::vec3 wiLocal = CosineSampleHemisphere({rng.uniform(), rng.uniform()});
            wiWorld = frame.worldFromLocal(wiLocal);
        }

        if (bounce >= rrStart) {
            float q = std::clamp(std::max({beta.x, beta.y, beta.z}), 0.05f, 0.95f);
            if (rng.uniform() > q) break;
            beta /= q;
        }

        ray.origin    = hitInfo->hitPos;
        ray.direction = wiWorld;

        if (beta.x < 1e-6f && beta.y < 1e-6f && beta.z < 1e-6f) break;
    }
    return L;
}
