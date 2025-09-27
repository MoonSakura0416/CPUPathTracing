#include "Material/conductor.h"
#include "Util/complex.h"

glm::vec3 FresnelConductor(const glm::vec3& ior, const glm::vec3& k, float cosThetaI)
{
    glm::vec3 fr{};
    cosThetaI = glm::clamp(cosThetaI, 0.0f, 1.0f);
    const float sin2ThetaI = 1.f - cosThetaI * cosThetaI;

    for (int i = 0; i < 3; ++i) {
        const Complex eta_c{ior[i], -k[i]};

        // Snell: sin^2 θ_t = (sin^2 θ_i) / eta_c^2
        const Complex eta2 = eta_c * eta_c;
        const Complex sin2ThetaT = Complex{sin2ThetaI} / eta2;

        // cos θ_t = sqrt(1 - sin^2 θ_t)
        const Complex cosThetaT = sqrt(Complex{1.f} - sin2ThetaT);

        // r_s: perpendicular polarization
        // r_s = (cosθi - η cosθt) / (cosθi + η cosθt)
        const Complex r_s_num = Complex{cosThetaI} - eta_c * cosThetaT;
        const Complex r_s_den = Complex{cosThetaI} + eta_c * cosThetaT;
        const Complex r_s = r_s_num / r_s_den;

        // r_p: parallel polarization
        // r_p = (η cosθi - cosθt) / (η cosθi + cosθt)
        const Complex r_p_num = eta_c * Complex{cosThetaI} - cosThetaT;
        const Complex r_p_den = eta_c * Complex{cosThetaI} + cosThetaT;
        const Complex r_p = r_p_num / r_p_den;

        // Fr = 0.5 * (|r_s|^2 + |r_p|^2)
        fr[i] = 0.5f * (norm(r_s) + norm(r_p));
    }
    return fr;
}

std::optional<BSDFSample> Conductor::sampleBSDF(const glm::vec3& hitPos, const glm::vec3& wi,
                                                const RNG& rng)
{
    constexpr float eps = 1e-6f;
    glm::vec3       microfacetNormal = {0, 1, 0};
    if (!microfacet_.isDeltaDistribution()) {
        microfacetNormal = microfacet_.sampleVisibleNormal(wi, rng);
    }
    const glm::vec3 fr = FresnelConductor(ior_, k_, glm::abs(glm::dot(wi, microfacetNormal)));
    // const glm::vec3 lightDir = -wi + 2.f * glm::dot(wi, microfacetNormal) * microfacetNormal;
    const glm::vec3 lightDir = glm::reflect(-wi, microfacetNormal);
    float           pdf = 0.f;
    glm::vec3       bsdf;
    if (microfacet_.isDeltaDistribution()) {
        pdf = 1.f;
        bsdf = fr / glm::max(glm::abs(lightDir.y), eps);
    } else {
        pdf = microfacet_.visibleNormalDistribution(wi, microfacetNormal) /
              (4.f * glm::abs(glm::dot(wi, microfacetNormal)));
        bsdf = fr * microfacet_.heightCorrelatedMaskingShadowing(lightDir, wi, microfacetNormal) *
               microfacet_.normalDistribution(microfacetNormal) /
               (4.f * glm::max(glm::abs(wi.y * lightDir.y), eps));
    }
    return BSDFSample{bsdf, pdf, lightDir};
}