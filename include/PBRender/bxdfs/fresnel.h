#pragma once

#include <PBRender/core/bxdf.h>
#include <PBRender/core/scattering.h>

namespace PBRender
{

// NormalizedFresnelBxDF Definition
class NormalizedFresnelBxDF : public BxDF {
public:
    // NormalizedFresnelBxDF Public Methods
    NormalizedFresnelBxDF() = default;
    NormalizedFresnelBxDF(float eta) : eta(eta) {}

    Optional<BSDFSample> Sample_f(Vector3f wo, float uc, Point2f u, TransportMode mode,
                        BxDFReflTransFlags sampleFlags) const {
        if (!(sampleFlags & BxDFReflTransFlags::Reflection))
            return {};

        // Cosine-sample the hemisphere, flipping the direction if necessary
        Vector3f wi = SampleCosineHemisphere(u);
        if (wo.z < 0)
            wi.z *= -1;
        return BSDFSample(f(wo, wi, mode), wi, PDF(wo, wi, mode, sampleFlags),
                          BxDFFlags::DiffuseReflection);
    }

    float PDF(Vector3f wo, Vector3f wi, TransportMode mode,
              BxDFReflTransFlags sampleFlags) const {
        if (!(sampleFlags & BxDFReflTransFlags::Reflection))
            return 0;
        return SameHemisphere(wo, wi) ? AbsCosTheta(wi) * InvPi : 0;
    }

    void Regularize() {}

    static constexpr const char *Name() { return "NormalizedFresnelBxDF"; }

    std::string ToString() const;

    BxDFFlags Flags() const {
        return BxDFFlags(BxDFFlags::Reflection | BxDFFlags::Diffuse);
    }

    Spectrum f(Vector3f wo, Vector3f wi, TransportMode mode) const {
        if (!SameHemisphere(wo, wi))
            return Spectrum(0.f);
        // Compute $\Sw$ factor for BSSRDF value
        float c = 1 - 2 * FresnelMoment1(1 / eta);
        Spectrum f((1 - FrDielectric(CosTheta(wi), eta)) / (c * Pi));

        // Update BSSRDF transmission term to account for adjoint light transport
        if (mode == TransportMode::Radiance)
            f *= Sqr(eta);

        return f;
    }

private:
    float eta;
};

} // namespace PBRender