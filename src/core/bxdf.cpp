#include <PBRender/core/bxdf.h>

namespace PBRender
{

std::string ToString(BxDFReflTransFlags flags) {
    if (flags == BxDFReflTransFlags::Unset)
        return "Unset";
    std::string s;
    if (flags & BxDFReflTransFlags::Reflection)
        s += "Reflection,";
    if (flags & BxDFReflTransFlags::Transmission)
        s += "Transmission,";
    return s;
}

std::string ToString(BxDFFlags flags) {
    if (flags == BxDFFlags::Unset)
        return "Unset";
    std::string s;
    if (flags & BxDFFlags::Reflection)
        s += "Reflection,";
    if (flags & BxDFFlags::Transmission)
        s += "Transmission,";
    if (flags & BxDFFlags::Diffuse)
        s += "Diffuse,";
    if (flags & BxDFFlags::Glossy)
        s += "Glossy,";
    if (flags & BxDFFlags::Specular)
        s += "Specular,";
    return s;
}

std::string ToString(TransportMode mode) {
    return mode == TransportMode::Radiance ? "Radiance" : "Importance";
}

// BxDF Method Definitions
Spectrum BxDF::rho(Vector3f wo, std::vector<float> &uc, std::vector<Point2f> &u2) const {
    if (wo.z == 0)
        return {};
    Spectrum r(0.);
    // DCHECK_EQ(uc.size(), u2.size());
    for (size_t i = 0; i < uc.size(); ++i) {
        // Compute estimate of $\rho_\roman{hd}$
        BSDFSample bs = Sample_f(wo, uc[i], u2[i]);
        if (bs.pdf > 0)
            r += bs.f * AbsCosTheta(bs.wi) / bs.pdf;
    }
    return r / uc.size();
}

Spectrum BxDF::rho(std::vector<Point2f> &u1, std::vector<float> &uc, std::vector<Point2f> &u2) const {
    // DCHECK_EQ(uc.size(), u1.size());
    // DCHECK_EQ(u1.size(), u2.size());
    Spectrum r(0.f);
    for (size_t i = 0; i < uc.size(); ++i) {
        // Compute estimate of $\rho_\roman{hh}$
        Vector3f wo = SampleUniformHemisphere(u1[i]);
        if (wo.z == 0)
            continue;
        float pdfo = UniformHemispherePDF();
        BSDFSample bs = Sample_f(wo, uc[i], u2[i]);
        if (bs.pdf > 0)
            r += bs.f * AbsCosTheta(bs.wi) * AbsCosTheta(wo) / (pdfo * bs.pdf);
    }
    return r / (Pi * uc.size());
}

} // namespace PBRender