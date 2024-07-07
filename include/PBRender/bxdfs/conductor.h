#pragma once

#include <PBRender/core/bxdf.h>
#include <PBRender/core/scattering.h>


namespace PBRender
{

// ConductorBxDF Definition
class ConductorBxDF : public BxDF {
  public:
    // ConductorBxDF Public Methods
    ConductorBxDF() = default;
    
    ConductorBxDF(const TrowbridgeReitzDistribution &mfDistrib, Spectrum eta,
                  Spectrum k)
        : mfDistrib(mfDistrib), eta(eta), k(k) {}
    
    BxDFFlags Flags() const {
        return mfDistrib.EffectivelySmooth() ? BxDFFlags::SpecularReflection
                                             : BxDFFlags::GlossyReflection;
    }
    
    BSDFSample Sample_f(
        Vector3f wo, float uc, Point2f u, TransportMode mode,
        BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const {
        if (!(sampleFlags & BxDFReflTransFlags::Reflection))
            return {};
        if (mfDistrib.EffectivelySmooth()) {
            // Sample perfect specular conductor BRDF
            Vector3f wi(-wo.x, -wo.y, wo.z);
            Spectrum f = FrComplex(AbsCosTheta(wi), eta, k) / AbsCosTheta(wi);
            return BSDFSample(f, wi, 1, BxDFFlags::SpecularReflection);
        }
        // Sample rough conductor BRDF
        // Sample microfacet normal $\wm$ and reflected direction $\wi$
        if (wo.z == 0)
            return {};
        Vector3f wm = mfDistrib.Sample_wm(wo, u);
        Vector3f wi = Reflect(wo, wm);
        if (!SameHemisphere(wo, wi))
            return {};

        // Compute PDF of _wi_ for microfacet reflection
        float pdf = mfDistrib.PDF(wo, wm) / (4 * AbsDot(wo, wm));

        float cosTheta_o = AbsCosTheta(wo), cosTheta_i = AbsCosTheta(wi);
        if (cosTheta_i == 0 || cosTheta_o == 0)
            return {};
        // Evaluate Fresnel factor _F_ for conductor BRDF
        Spectrum F = FrComplex(AbsDot(wo, wm), eta, k);

        Spectrum f =
            mfDistrib.D(wm) * F * mfDistrib.G(wo, wi) / (4 * cosTheta_i * cosTheta_o);
        return BSDFSample(f, wi, pdf, BxDFFlags::GlossyReflection);
    }

    
    Spectrum f(Vector3f wo, Vector3f wi, TransportMode mode) const {
        if (!SameHemisphere(wo, wi))
            return {};
        if (mfDistrib.EffectivelySmooth())
            return {};
        // Evaluate rough conductor BRDF
        // Compute cosines and $\wm$ for conductor BRDF
        float cosTheta_o = AbsCosTheta(wo), cosTheta_i = AbsCosTheta(wi);
        if (cosTheta_i == 0 || cosTheta_o == 0)
            return {};
        Vector3f wm = wi + wo;
        if (wm.LengthSquared() == 0)
            return {};
        wm = Normalize(wm);

        // Evaluate Fresnel factor _F_ for conductor BRDF
        Spectrum F = FrComplex(AbsDot(wo, wm), eta, k);

        return mfDistrib.D(wm) * F * mfDistrib.G(wo, wi) / (4 * cosTheta_i * cosTheta_o);
    }

    
    float PDF(Vector3f wo, Vector3f wi, TransportMode mode,
              BxDFReflTransFlags sampleFlags) const {
        if (!(sampleFlags & BxDFReflTransFlags::Reflection))
            return 0;
        if (!SameHemisphere(wo, wi))
            return 0;
        if (mfDistrib.EffectivelySmooth())
            return 0;
        // Evaluate sampling PDF of rough conductor BRDF
        Vector3f wm = wo + wi;
        // CHECK_RARE(1e-5f, LengthSquared(wm) == 0);
        if (wm.LengthSquared() == 0)
            return 0;
        wm = FaceForward(Normalize(wm), Normal3f(0, 0, 1));
        return mfDistrib.PDF(wo, wm) / (4 * AbsDot(wo, wm));
    }
    
    static constexpr const char *Name() { return "ConductorBxDF"; }
    std::string ToString() const;

    void Regularize() { mfDistrib.Regularize(); }

  private:
    // ConductorBxDF Private Members
    TrowbridgeReitzDistribution mfDistrib;
    Spectrum eta, k;
};


} // namespace PBRender