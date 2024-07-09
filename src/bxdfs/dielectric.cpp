#include <PBRender/bxdfs/dielectric.h>

namespace PBRender
{

Optional<BSDFSample> DielectricBxDF::Sample_f(
    Vector3f wo, float uc, Point2f u, TransportMode mode,
    BxDFReflTransFlags sampleFlags) const {
    if (eta == 1 || mfDistrib.EffectivelySmooth()) {
        // Sample perfect specular dielectric BSDF
        float R = FrDielectric(CosTheta(wo), eta), T = 1 - R;
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
            // Sample perfect specular dielectric BTDF
            // Compute ray direction for specular transmission
            Vector3f wi;
            float etap;
            bool valid = Refract(wo, Normal3f(0, 0, 1), eta, &etap, &wi);
            // CHECK_RARE(1e-5f, !valid);
            if (!valid)
                return {};

            Spectrum ft(T / AbsCosTheta(wi));
            // Account for non-symmetry with transmission to different medium
            if (mode == TransportMode::Radiance)
                ft /= Sqr(etap);

            return BSDFSample(ft, wi, pt / (pr + pt), BxDFFlags::SpecularTransmission,
                              etap);
        }

    } else {
        // Sample rough dielectric BSDF
        Vector3f wm = mfDistrib.Sample_wm(wo, u);
        float R = FrDielectric(Dot(wo, wm), eta);
        float T = 1 - R;
        // Compute probabilities _pr_ and _pt_ for sampling reflection and transmission
        float pr = R, pt = T;
        if (!(sampleFlags & BxDFReflTransFlags::Reflection))
            pr = 0;
        if (!(sampleFlags & BxDFReflTransFlags::Transmission))
            pt = 0;
        if (pr == 0 && pt == 0)
            return {};

        float pdf;
        if (uc < pr / (pr + pt)) {
            // Sample reflection at rough dielectric interface
            Vector3f wi = Reflect(wo, wm);
            if (!SameHemisphere(wo, wi))
                return {};
            // Compute PDF of rough dielectric reflection
            pdf = mfDistrib.PDF(wo, wm) / (4 * AbsDot(wo, wm)) * pr / (pr + pt);

            // DCHECK(!IsNaN(pdf));
            Spectrum f(mfDistrib.D(wm) * mfDistrib.G(wo, wi) * R /
                              (4 * CosTheta(wi) * CosTheta(wo)));
            return BSDFSample(f, wi, pdf, BxDFFlags::GlossyReflection);

        } else {
            // Sample transmission at rough dielectric interface
            float etap;
            Vector3f wi;
            bool tir = !Refract(wo, (Normal3f)wm, eta, &etap, &wi);
            // CHECK_RARE(1e-5f, tir);
            if (SameHemisphere(wo, wi) || wi.z == 0 || tir)
                return {};
            // Compute PDF of rough dielectric transmission
            float denom = Sqr(Dot(wi, wm) + Dot(wo, wm) / etap);
            float dwm_dwi = AbsDot(wi, wm) / denom;
            pdf = mfDistrib.PDF(wo, wm) * dwm_dwi * pt / (pr + pt);

            // CHECK(!IsNaN(pdf));
            // Evaluate BRDF and return _BSDFSample_ for rough transmission
            Spectrum ft(T * mfDistrib.D(wm) * mfDistrib.G(wo, wi) *
                               std::abs(Dot(wi, wm) * Dot(wo, wm) /
                                        (CosTheta(wi) * CosTheta(wo) * denom)));
            // Account for non-symmetry with transmission to different medium
            if (mode == TransportMode::Radiance)
                ft /= Sqr(etap);

            return BSDFSample(ft, wi, pdf, BxDFFlags::GlossyTransmission, etap);
        }
    }
}

Spectrum DielectricBxDF::f(Vector3f wo, Vector3f wi, TransportMode mode) const {
    if (eta == 1 || mfDistrib.EffectivelySmooth())
        return Spectrum(0.f);
    // Evaluate rough dielectric BSDF
    // Compute generalized half vector _wm_
    float cosTheta_o = CosTheta(wo), cosTheta_i = CosTheta(wi);
    bool reflect = cosTheta_i * cosTheta_o > 0;
    float etap = 1;
    if (!reflect)
        etap = cosTheta_o > 0 ? eta : (1 / eta);
    Vector3f wm = wi * etap + wo;
    // CHECK_RARE(1e-5f, LengthSquared(wm) == 0);
    if (cosTheta_i == 0 || cosTheta_o == 0 || wm.LengthSquared() == 0)
        return {};
    wm = FaceForward(Normalize(wm), Normal3f(0, 0, 1));

    // Discard backfacing microfacets
    if (Dot(wm, wi) * cosTheta_i < 0 || Dot(wm, wo) * cosTheta_o < 0)
        return {};

    float F = FrDielectric(Dot(wo, wm), eta);
    if (reflect) {
        // Compute reflection at rough dielectric interface
        return Spectrum(mfDistrib.D(wm) * mfDistrib.G(wo, wi) * F /
                               std::abs(4 * cosTheta_i * cosTheta_o));

    } else {
        // Compute transmission at rough dielectric interface
        float denom = Sqr(Dot(wi, wm) + Dot(wo, wm) / etap) * cosTheta_i * cosTheta_o;
        float ft = mfDistrib.D(wm) * (1 - F) * mfDistrib.G(wo, wi) *
                   std::abs(Dot(wi, wm) * Dot(wo, wm) / denom);
        // Account for non-symmetry with transmission to different medium
        if (mode == TransportMode::Radiance)
            ft /= Sqr(etap);

        return Spectrum(ft);
    }
}

float DielectricBxDF::PDF(Vector3f wo, Vector3f wi, TransportMode mode,
                          BxDFReflTransFlags sampleFlags) const {
    if (eta == 1 || mfDistrib.EffectivelySmooth())
        return 0;
    // Evaluate sampling PDF of rough dielectric BSDF
    // Compute generalized half vector _wm_
    float cosTheta_o = CosTheta(wo), cosTheta_i = CosTheta(wi);
    bool reflect = cosTheta_i * cosTheta_o > 0;
    float etap = 1;
    if (!reflect)
        etap = cosTheta_o > 0 ? eta : (1 / eta);
    Vector3f wm = wi * etap + wo;
    // CHECK_RARE(1e-5f, LengthSquared(wm) == 0);
    if (cosTheta_i == 0 || cosTheta_o == 0 || wm.LengthSquared() == 0)
        return {};
    wm = FaceForward(Normalize(wm), Normal3f(0, 0, 1));

    // Discard backfacing microfacets
    if (Dot(wm, wi) * cosTheta_i < 0 || Dot(wm, wo) * cosTheta_o < 0)
        return {};

    // Determine Fresnel reflectance of rough dielectric boundary
    float R = FrDielectric(Dot(wo, wm), eta);
    float T = 1 - R;

    // Compute probabilities _pr_ and _pt_ for sampling reflection and transmission
    float pr = R, pt = T;
    if (!(sampleFlags & BxDFReflTransFlags::Reflection))
        pr = 0;
    if (!(sampleFlags & BxDFReflTransFlags::Transmission))
        pt = 0;
    if (pr == 0 && pt == 0)
        return {};

    // Return PDF for rough dielectric
    float pdf;
    if (reflect) {
        // Compute PDF of rough dielectric reflection
        pdf = mfDistrib.PDF(wo, wm) / (4 * AbsDot(wo, wm)) * pr / (pr + pt);

    } else {
        // Compute PDF of rough dielectric transmission
        float denom = Sqr(Dot(wi, wm) + Dot(wo, wm) / etap);
        float dwm_dwi = AbsDot(wi, wm) / denom;
        pdf = mfDistrib.PDF(wo, wm) * dwm_dwi * pt / (pr + pt);
    }
    return pdf;
}

std::string DielectricBxDF::ToString() const {
    // return StringPrintf("[ DielectricBxDF eta: %f mfDistrib: %s ]", eta,
    //                     mfDistrib.ToString());

    std::ostringstream ss;
    ss << "[ DielectricBxDF eta: " << eta << " "
       << "mfDistrib: " << mfDistrib.ToString() << " ]";

    return ss.str();
}

std::string ThinDielectricBxDF::ToString() const {
    // return StringPrintf("[ ThinDielectricBxDF eta: %f ]", eta);

    std::ostringstream ss;
    ss << "[ ThinDielectricBxDF eta: " << eta << " ]";

    return ss.str();
}

} // namespace PBRender