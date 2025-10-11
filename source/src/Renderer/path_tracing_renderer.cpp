#include "Renderer/path_tracing_renderer.h"
#include "Util/frame.h"

// β = 2
float PowerHeuristic(int nf, float fPdf, int ng, float gPdf)
{
    const float f = nf * fPdf;
    const float g = ng * gPdf;
    return (f * f) / (f * f + g * g);
}

glm::vec3 PathTracingRenderer::renderPixel(const glm::ivec2& pixelCoord, RNG& rng)
{
    Ray           ray = camera_.getRay(pixelCoord, {rng.uniform(), rng.uniform()});
    glm::vec3     beta(1.f, 1.f, 1.f);
    glm::vec3     L(0.f, 0.f, 0.f);
    constexpr int rrStart = 5;

    bool lastIsSpecular = true;
    bool allowMISCompensation = true;
    float lastBSDFPdf = 0.f;
    float etaScale = 1.f;

    for (int bounce = 0;; ++bounce) {
        auto hitInfo = scene_.intersect(ray);
        if (!hitInfo) {
            glm::vec3 lightDir = glm::normalize(ray.direction);
            glm::vec3 lightPoint = ray.origin + scene_.getRadius() * 2.f * lightDir;
            if (lastIsSpecular) {
                for (const auto& infiniteLight: scene_.getInfiniteLights()) {
                    L += beta * infiniteLight->getEmission(ray.origin, lightPoint, -lightDir);
                }
            } else {
                for (const auto& infiniteLight: scene_.getInfiniteLights()) {
                    float lightSourceProb = scene_.getLightSampler().getProb(*infiniteLight, allowMISCompensation);
                    float lightPdf = infiniteLight->PDF(ray.origin, lightPoint, -lightDir, allowMISCompensation);
                    float weight = PowerHeuristic(1, lastBSDFPdf, 1, lightPdf * lightSourceProb);
                    L += weight * beta * infiniteLight->getEmission(ray.origin, ray.origin + scene_.getRadius() * 2 * lightDir, -lightDir);
                }
            }
            break;
        }

        if (hitInfo->material && hitInfo->material->areaLight) {
            float weight = 1.f;
            if (!lastIsSpecular) {
                float lightSourceProb = scene_.getLightSampler().getProb(*hitInfo->material->areaLight, allowMISCompensation);
                float lightPdf = hitInfo->material->areaLight->PDF(ray.origin, hitInfo->hitPos, hitInfo->normal, !lastIsSpecular);
                weight = PowerHeuristic(1, lastBSDFPdf, 1, lightPdf * lightSourceProb);
            }
                L += weight * beta * hitInfo->material->areaLight->getEmission(ray.origin, hitInfo->hitPos,hitInfo->normal);
            }

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

            lastIsSpecular = hitInfo->material->isDeltaDistribution();
            if (!lastIsSpecular) {
                auto lightSourceSample = scene_.getLightSampler().sample(rng.uniform(), allowMISCompensation);
                if (lightSourceSample.has_value()) {
                    auto lightSample = lightSourceSample->light.get().lightSample(
                        hitInfo->hitPos, scene_.getRadius(), rng);
                    if (lightSample.has_value() && (!scene_.intersect({ hitInfo->hitPos, lightSample->position - hitInfo->hitPos }, 1e-5, 1.f - 1e-5))) {
                        glm::vec3 lightDirectionLocal = frame.localFromWorld(lightSample->direction);
                        float bsdfPdf = hitInfo->material->PDF(hitInfo->hitPos, lightDirectionLocal, localWi);
                        float weight = PowerHeuristic(1, lightSample->pdf * lightSourceSample->prob, 1, bsdfPdf);
                        L +=
                            weight * beta *
                            hitInfo->material->BSDF(hitInfo->hitPos, lightDirectionLocal, localWi) *
                            glm::abs(lightDirectionLocal.y) * lightSample->emission /
                            (lightSample->pdf * lightSourceSample->prob);
                    }
                }
            }
            auto bsdfSample = hitInfo->material->sampleBSDF(hitInfo->hitPos, localWi, rng);
            if (!bsdfSample)
                break;
            lastBSDFPdf = bsdfSample->pdf;
            etaScale *= bsdfSample->etaScale;
            beta *= bsdfSample->bsdf * std::abs(bsdfSample->lightDir.y) / bsdfSample->pdf;
            lightDir = bsdfSample->lightDir;
        } else {
            break;
        }

        lightDir = frame.worldFromLocal(lightDir);

        if (bounce >= rrStart) {
            //glm::vec3 betaQ = beta * etaScale;
            glm::vec3 betaQ = beta;
            float q = std::clamp(std::max({betaQ.x, betaQ.y, betaQ.z}), 0.05f, 0.95f);
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
