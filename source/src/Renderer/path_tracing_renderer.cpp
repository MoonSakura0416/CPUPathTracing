#include "Renderer/path_tracing_renderer.h"
#include "Util/frame.h"

glm::vec3 PathTracingRenderer::renderPixel(const glm::ivec2& pixelCoord, RNG& rng)
{
    Ray           ray = camera_.getRay(pixelCoord, {rng.uniform(), rng.uniform()});
    glm::vec3     beta(1.f, 1.f, 1.f);
    glm::vec3     L(0.f, 0.f, 0.f);
    constexpr int rrStart = 5;

    bool lastIsSpecular = true;
    glm::vec3 lastSurfacePoint = ray.origin;

    for (int bounce = 0;; ++bounce) {
        auto hitInfo = scene_.intersect(ray);
        if (!hitInfo) {
            if (lastIsSpecular) {
                for (const auto& infiniteLight: scene_.getInfiniteLights()) {
                    glm::vec3 lightDir = glm::normalize(ray.direction);
                    L += beta * infiniteLight->getEmission(lastSurfacePoint, lastSurfacePoint + scene_.getRadius() * 2 * lightDir, -lightDir);
                }
            }
            break;
        }

        if (lastIsSpecular && hitInfo->material && hitInfo->material->areaLight) {
            L += beta * hitInfo->material->areaLight->getEmission(lastSurfacePoint, hitInfo->hitPos, hitInfo->normal);
        }
        lastSurfacePoint = hitInfo->hitPos;
        // BRDF = albedo / PI
        // PDF = cos_theta / PI
        // final_term = (albedo / PI) * cos_theta * (1 / PDF) = albedo

        Frame     frame(hitInfo->normal);
        glm::vec3 lightDir;

        if (hitInfo->material) {
            glm::vec3 localWi = frame.localFromWorld(-ray.direction);
            if (localWi.y == 0) {
                ray.origin = hitInfo->hitPos;
                continue;
            }

            if (hitInfo->material->isDeltaDistribution()) {
                lastIsSpecular = true;
            } else {
                lastIsSpecular = false;
                auto lightSourceSample = scene_.getLightSampler().sample(rng.uniform());
                if (lightSourceSample.has_value()) {
                    auto lightSample = lightSourceSample->light.get().lightSample(
                        hitInfo->hitPos, scene_.getRadius(), rng);
                    if (lightSample.has_value() && (!scene_.intersect({ hitInfo->hitPos, lightSample->position - hitInfo->hitPos }, 1e-5, 1.f - 1e-5))) {
                        glm::vec3 lightDirectionLocal = frame.localFromWorld(lightSample->direction);
                        L += beta *
                             hitInfo->material->BSDF(hitInfo->hitPos, lightDirectionLocal, localWi) *
                             glm::abs(lightDirectionLocal.y) * lightSample->emission /
                             (lightSample->pdf * lightSourceSample->prob);
                    }

                }
            }

            auto bsdfSample = hitInfo->material->sampleBSDF(hitInfo->hitPos, localWi, rng);
            if (!bsdfSample)
                break;
            beta *= bsdfSample->bsdf * std::abs(bsdfSample->lightDir.y) / bsdfSample->pdf;
            lightDir = bsdfSample->lightDir;
        } else {
            break;
        }

        lightDir = frame.worldFromLocal(lightDir);

        if (bounce >= rrStart) {
            float q = std::clamp(std::max({beta.x, beta.y, beta.z}), 0.05f, 0.95f);
            if (rng.uniform() > q)
                break;
            beta /= q;
        }

        ray.origin = hitInfo->hitPos;
        ray.direction = lightDir;

        if (beta.x < 1e-6f && beta.y < 1e-6f && beta.z < 1e-6f)
            break;
    }
    return L;
}
