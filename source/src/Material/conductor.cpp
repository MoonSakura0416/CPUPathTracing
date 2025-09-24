#include "Material/conductor.h"
#include "Util/complex.h"

std::optional<BSDFSample> Conductor::sampleBSDF(const glm::vec3& hitPos, const glm::vec3& wi,
                         const RNG& rng)
{
    const float cosThetaI = glm::clamp(wi.y, 0.f, 1.f);
    const float sin2ThetaI = 1.f - cosThetaI * cosThetaI;

    // auto sqrMag = [](Complex z) -> float {
    //     // |z|^2 = real^2 + imag^2
    //     return z.real * z.real + z.imag * z.imag;
    // };

    glm::vec3 fr(0.0f);

    for (int i = 0; i < 3; ++i) {
        const Complex eta_c{ ior_[i], -k_[i] };

        // Snell: sin^2 θ_t = (sin^2 θ_i) / eta_c^2
        const Complex eta2 = eta_c * eta_c;
        const Complex sin2ThetaT = Complex{ sin2ThetaI } / eta2;

        // cos θ_t = sqrt(1 - sin^2 θ_t)
        const Complex cosThetaT = sqrt(Complex{ 1.f } - sin2ThetaT);

        // r_s: perpendicular polarization
        // r_s = (cosθi - η cosθt) / (cosθi + η cosθt)
        const Complex r_s_num = Complex{ cosThetaI } - eta_c * cosThetaT;
        const Complex r_s_den = Complex{ cosThetaI } + eta_c * cosThetaT;
        const Complex r_s = r_s_num / r_s_den;

        // r_p: parallel polarization
        // r_p = (η cosθi - cosθt) / (η cosθi + cosθt)
        const Complex r_p_num = eta_c * Complex{ cosThetaI } - cosThetaT;
        const Complex r_p_den = eta_c * Complex{ cosThetaI } + cosThetaT;
        const Complex r_p = r_p_num / r_p_den;

        // Fr = 0.5 * (|r_s|^2 + |r_p|^2)
        fr[i] = 0.5f * (norm(r_s) + norm(r_p));
    }
    const glm::vec3 lightDir = {-wi.x, wi.y, -wi.z};
    constexpr float pdf = 1.f;
    const glm::vec3 bsdf = fr / std::abs(lightDir.y);
    return BSDFSample{bsdf, pdf, lightDir};
}