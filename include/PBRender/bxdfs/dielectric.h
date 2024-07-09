#pragma once

#include <PBRender/core/bxdf.h>
#include <PBRender/core/scattering.h>

namespace PBRender
{

// DielectricBxDF Definition
class DielectricBxDF : public BxDF {
public:
    // DielectricBxDF Public Methods
    DielectricBxDF() = default;
    DielectricBxDF(float eta, TrowbridgeReitzDistribution mfDistrib)
        : eta(eta), mfDistrib(mfDistrib) {}

    
    BxDFFlags Flags() const {
        BxDFFlags flags = (eta == 1) ? BxDFFlags::Transmission
                                     : (BxDFFlags::Reflection | BxDFFlags::Transmission);
        return flags |
               (mfDistrib.EffectivelySmooth() ? BxDFFlags::Specular : BxDFFlags::Glossy);
    }

    Optional<BSDFSample> Sample_f(
        Vector3f wo, float uc, Point2f u, TransportMode mode,
        BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const;
    
    Spectrum f(Vector3f wo, Vector3f wi, TransportMode mode) const;
    
    float PDF(Vector3f wo, Vector3f wi, TransportMode mode,
              BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const;
    
    static constexpr const char *Name() { return "DielectricBxDF"; }

    std::string ToString() const;

    void Regularize() { mfDistrib.Regularize(); }

private:
    // DielectricBxDF Private Members
    float eta;
    TrowbridgeReitzDistribution mfDistrib;
};

// ThinDielectricBxDF Definition
class ThinDielectricBxDF : public BxDF {
public:
    ThinDielectricBxDF() = default;
    ThinDielectricBxDF(float eta) : eta(eta) {}

    
    Spectrum f(Vector3f wo, Vector3f wi, TransportMode mode) const {
        return Spectrum(0);
    }
    
    Optional<BSDFSample> Sample_f(Vector3f wo, float uc, Point2f u,
                                        TransportMode mode,
                                        BxDFReflTransFlags sampleFlags) const {
        float R = FrDielectric(AbsCosTheta(wo), eta), T = 1 - R;
        // Compute _R_ and _T_ accounting for scattering between interfaces
        if (R < 1) {
            R += Sqr(T) * R / (1 - Sqr(R));
            T = 1 - R;
        }

        // Compute probabilities _pr_ and _pt_ for sampling reflection and transmission
        float pr = R, pt = T;
        if (!(sampleFlags & BxDFReflTransFlags::Reflection))
            pr = 0;
        if (!(sampleFlags & BxDFReflTransFlags::Transmission))
            pt = 0;
        if (pr == 0 && pt == 0)
            return {};

        if (uc < pr / (pr + pt)) {
            // Sample perfect specular dielectric BRDF
            Vector3f wi(-wo.x, -wo.y, wo.z);
            Spectrum fr(R / AbsCosTheta(wi));
            return BSDFSample(fr, wi, pr / (pr + pt), BxDFFlags::SpecularReflection);

        } else {
            // Sample perfect specular transmission at thin dielectric interface
            Vector3f wi = -wo;
            Spectrum ft(T / AbsCosTheta(wi));
            return BSDFSample(ft, wi, pt / (pr + pt), BxDFFlags::SpecularTransmission);
        }
    }

    float PDF(Vector3f wo, Vector3f wi, TransportMode mode,
              BxDFReflTransFlags sampleFlags) const {
        return 0;
    }

    static constexpr const char *Name() { return "ThinDielectricBxDF"; }

    std::string ToString() const;

    void Regularize() { /* TODO */
    }

    BxDFFlags Flags() const {
        return (BxDFFlags::Reflection | BxDFFlags::Transmission | BxDFFlags::Specular);
    }

private:
    float eta;
};


} // namespace PBRender