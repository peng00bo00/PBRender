#include "reflection.h"
#include "spectrum.h"
#include "sampler.h"
#include "sampling.h"
// #include "interpolation.h"
#include "scene.h"
#include "interaction.h"
// #include "stats.h"
#include <stdarg.h>

namespace PBRender {

// BxDF Utility Functions
float FrDielectric(float cosThetaI, float etaI, float etaT) {
    cosThetaI = Clamp(cosThetaI, -1, 1);
    // Potentially swap indices of refraction
    bool entering = cosThetaI > 0.f;
    if (!entering) {
        std::swap(etaI, etaT);
        cosThetaI = std::abs(cosThetaI);
    }

    // Compute _cosThetaT_ using Snell's law
    float sinThetaI = std::sqrt(std::max((float)0, 1 - cosThetaI * cosThetaI));
    float sinThetaT = etaI / etaT * sinThetaI;

    // Handle total internal reflection
    if (sinThetaT >= 1) return 1;
    float cosThetaT = std::sqrt(std::max((float)0, 1 - sinThetaT * sinThetaT));
    float Rparl = ((etaT * cosThetaI) - (etaI * cosThetaT)) /
                  ((etaT * cosThetaI) + (etaI * cosThetaT));
    float Rperp = ((etaI * cosThetaI) - (etaT * cosThetaT)) /
                  ((etaI * cosThetaI) + (etaT * cosThetaT));
    return (Rparl * Rparl + Rperp * Rperp) / 2;
}

// https://seblagarde.wordpress.com/2013/04/29/memo-on-fresnel-equations/
Spectrum FrConductor(float cosThetaI, const Spectrum &etai,
                     const Spectrum &etat, const Spectrum &k) {
    cosThetaI = Clamp(cosThetaI, -1, 1);
    Spectrum eta = etat / etai;
    Spectrum etak = k / etai;

    float cosThetaI2 = cosThetaI * cosThetaI;
    float sinThetaI2 = 1. - cosThetaI2;
    Spectrum eta2 = eta * eta;
    Spectrum etak2 = etak * etak;

    Spectrum t0 = eta2 - etak2 - sinThetaI2;
    Spectrum a2plusb2 = Sqrt(t0 * t0 + 4 * eta2 * etak2);
    Spectrum t1 = a2plusb2 + cosThetaI2;
    Spectrum a = Sqrt(0.5f * (a2plusb2 + t0));
    Spectrum t2 = (float)2 * cosThetaI * a;
    Spectrum Rs = (t1 - t2) / (t1 + t2);

    Spectrum t3 = cosThetaI2 * a2plusb2 + sinThetaI2 * sinThetaI2;
    Spectrum t4 = t2 * sinThetaI2;
    Spectrum Rp = Rs * (t3 - t4) / (t3 + t4);

    return 0.5 * (Rp + Rs);
}

Spectrum LambertianReflection::f(const Vector3f &wo, const Vector3f &wi) const {
    return R * InvPi;
}

std::string LambertianReflection::ToString() const {
    return std::string("[ LambertianReflection R: ") + R.ToString() +
           std::string(" ]");
}

// BxDF Method Definitions
Spectrum BxDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &u,
                        float *pdf, BxDFType *sampledType) const {
    // Cosine-sample the hemisphere, flipping the direction if necessary
    *wi = CosineSampleHemisphere(u);
    if (wo.z < 0) wi->z *= -1;
    *pdf = Pdf(wo, *wi);
    return f(wo, *wi);
}

float BxDF::Pdf(const Vector3f &wo, const Vector3f &wi) const {
    return SameHemisphere(wo, wi) ? AbsCosTheta(wi) * InvPi : 0;
}

Spectrum BxDF::rho(const Vector3f &w, int nSamples, const Point2f *u) const {
    Spectrum r(0.);
    for (int i = 0; i < nSamples; ++i) {
        // Estimate one term of $\rho_\roman{hd}$
        Vector3f wi;
        float pdf = 0;
        Spectrum f = Sample_f(w, &wi, u[i], &pdf);
        if (pdf > 0) r += f * AbsCosTheta(wi) / pdf;
    }
    return r / nSamples;
}

Spectrum BxDF::rho(int nSamples, const Point2f *u1, const Point2f *u2) const {
    Spectrum r(0.f);
    for (int i = 0; i < nSamples; ++i) {
        // Estimate one term of $\rho_\roman{hh}$
        Vector3f wo, wi;
        wo = UniformSampleHemisphere(u1[i]);
        float pdfo = UniformHemispherePdf(), pdfi = 0;
        Spectrum f = Sample_f(wo, &wi, u2[i], &pdfi);
        if (pdfi > 0)
            r += f * AbsCosTheta(wi) * AbsCosTheta(wo) / (pdfo * pdfi);
    }
    return r / (Pi * nSamples);
}

// BSDF Method Definitions
Spectrum BSDF::f(const Vector3f &woW, const Vector3f &wiW,
                 BxDFType flags) const {
    // ProfilePhase pp(Prof::BSDFEvaluation);
    Vector3f wi = WorldToLocal(wiW), wo = WorldToLocal(woW);
    if (wo.z == 0) return 0.;
    bool reflect = Dot(wiW, ng) * Dot(woW, ng) > 0;
    Spectrum f(0.f);
    for (int i = 0; i < nBxDFs; ++i)
        if (bxdfs[i]->MatchesFlags(flags) &&
            ((reflect && (bxdfs[i]->type & BSDF_REFLECTION)) ||
             (!reflect && (bxdfs[i]->type & BSDF_TRANSMISSION))))
            f += bxdfs[i]->f(wo, wi);
    return f;
}

Spectrum BSDF::rho(int nSamples, const Point2f *samples1,
                   const Point2f *samples2, BxDFType flags) const {
    Spectrum ret(0.f);
    for (int i = 0; i < nBxDFs; ++i)
        if (bxdfs[i]->MatchesFlags(flags))
            ret += bxdfs[i]->rho(nSamples, samples1, samples2);
    return ret;
}

Spectrum BSDF::rho(const Vector3f &woWorld, int nSamples, const Point2f *samples,
                   BxDFType flags) const {
    Vector3f wo = WorldToLocal(woWorld);
    Spectrum ret(0.f);
    for (int i = 0; i < nBxDFs; ++i)
        if (bxdfs[i]->MatchesFlags(flags))
            ret += bxdfs[i]->rho(wo, nSamples, samples);
    return ret;
}

Spectrum BSDF::Sample_f(const Vector3f &woWorld, Vector3f *wiWorld,
                        const Point2f &u, float *pdf, BxDFType type,
                        BxDFType *sampledType) const {
    // ProfilePhase pp(Prof::BSDFSampling);

    // Choose which _BxDF_ to sample
    int matchingComps = NumComponents(type);
    if (matchingComps == 0) {
        *pdf = 0;
        if (sampledType) *sampledType = BxDFType(0);
        return Spectrum(0);
    }
    int comp =
        std::min((int)std::floor(u[0] * matchingComps), matchingComps - 1);

    // Get _BxDF_ pointer for chosen component
    std::shared_ptr<BxDF> bxdf = nullptr;
    int count = comp;
    for (int i = 0; i < nBxDFs; ++i)
        if (bxdfs[i]->MatchesFlags(type) && count-- == 0) {
            bxdf = bxdfs[i];
            break;
        }
    // CHECK(bxdf != nullptr);
    assert(bxdf != nullptr);
    // VLOG(2) << "BSDF::Sample_f chose comp = " << comp << " / matching = " <<
    //     matchingComps << ", bxdf: " << bxdf->ToString();

    // Remap _BxDF_ sample _u_ to $[0,1)^2$
    Point2f uRemapped(std::min(u[0] * matchingComps - comp, OneMinusEpsilon),
                      u[1]);

    // Sample chosen _BxDF_
    Vector3f wi, wo = WorldToLocal(woWorld);
    if (wo.z == 0) return 0.;
    *pdf = 0;
    if (sampledType) *sampledType = bxdf->type;
    Spectrum f = bxdf->Sample_f(wo, &wi, uRemapped, pdf, sampledType);
    // VLOG(2) << "For wo = " << wo << ", sampled f = " << f << ", pdf = "
    //         << *pdf << ", ratio = " << ((*pdf > 0) ? (f / *pdf) : Spectrum(0.))
    //         << ", wi = " << wi;
    if (*pdf == 0) {
        if (sampledType) *sampledType = BxDFType(0);
        return 0;
    }
    *wiWorld = LocalToWorld(wi);

    // Compute overall PDF with all matching _BxDF_s
    if (!(bxdf->type & BSDF_SPECULAR) && matchingComps > 1)
        for (int i = 0; i < nBxDFs; ++i)
            if (bxdfs[i] != bxdf && bxdfs[i]->MatchesFlags(type))
                *pdf += bxdfs[i]->Pdf(wo, wi);
    if (matchingComps > 1) *pdf /= matchingComps;

    // Compute value of BSDF for sampled direction
    if (!(bxdf->type & BSDF_SPECULAR)) {
        bool reflect = Dot(*wiWorld, ng) * Dot(woWorld, ng) > 0;
        f = 0.;
        for (int i = 0; i < nBxDFs; ++i)
            if (bxdfs[i]->MatchesFlags(type) &&
                ((reflect && (bxdfs[i]->type & BSDF_REFLECTION)) ||
                 (!reflect && (bxdfs[i]->type & BSDF_TRANSMISSION))))
                f += bxdfs[i]->f(wo, wi);
    }
    // VLOG(2) << "Overall f = " << f << ", pdf = " << *pdf << ", ratio = "
    //         << ((*pdf > 0) ? (f / *pdf) : Spectrum(0.));
    return f;
}

float BSDF::Pdf(const Vector3f &woWorld, const Vector3f &wiWorld,
                BxDFType flags) const {
    // ProfilePhase pp(Prof::BSDFPdf);
    if (nBxDFs == 0.f) return 0.f;
    Vector3f wo = WorldToLocal(woWorld), wi = WorldToLocal(wiWorld);
    if (wo.z == 0) return 0.;
    float pdf = 0.f;
    int matchingComps = 0;
    for (int i = 0; i < nBxDFs; ++i)
        if (bxdfs[i]->MatchesFlags(flags)) {
            ++matchingComps;
            pdf += bxdfs[i]->Pdf(wo, wi);
        }
    float v = matchingComps > 0 ? pdf / matchingComps : 0.f;
    return v;
}

std::string BSDF::ToString() const {
    // std::string s = StringPrintf("[ BSDF eta: %f nBxDFs: %d", eta, nBxDFs);
    // for (int i = 0; i < nBxDFs; ++i)
    //     s += StringPrintf("\n  bxdfs[%d]: ", i) + bxdfs[i]->ToString();
    // return s + std::string(" ]");

    std::ostringstream ss;
    ss << "[ BSDF eta: " << eta << " nBxDFs: " << nBxDFs;

    for (int i = 0; i < nBxDFs; ++i)
        ss << "\n bxdfs[" << i << "]: " << bxdfs[i]->ToString();
    ss << " ]";
    return ss.str();
}

Fresnel::~Fresnel() {}

Spectrum FresnelConductor::Evaluate(float cosThetaI) const {
    return FrConductor(std::abs(cosThetaI), etaI, etaT, k);
}

std::string FresnelConductor::ToString() const {
    return std::string("[ FresnelConductor etaI: ") + etaI.ToString() +
           std::string(" etaT: ") + etaT.ToString() + std::string(" k: ") +
           k.ToString() + std::string(" ]");
}

Spectrum FresnelDielectric::Evaluate(float cosThetaI) const {
    return FrDielectric(cosThetaI, etaI, etaT);
}

std::string FresnelDielectric::ToString() const {
    // return StringPrintf("[ FrenselDielectric etaI: %f etaT: %f ]", etaI, etaT);
    std::ostringstream ss;
    ss << "[ FrenselDielectric etaI: " << etaI << " etaT: " << etaT << " ]";

    return ss.str();
}

Spectrum SpecularReflection::Sample_f(const Vector3f &wo, Vector3f *wi,
                                      const Point2f &sample, float *pdf,
                                      BxDFType *sampledType) const {
    // Compute perfect specular reflection direction
    *wi = Vector3f(-wo.x, -wo.y, wo.z);
    *pdf = 1;
    return fresnel->Evaluate(CosTheta(*wi)) * R / AbsCosTheta(*wi);
}

std::string SpecularReflection::ToString() const {
    return std::string("[ SpecularReflection R: ") + R.ToString() +
           std::string(" fresnel: ") + fresnel->ToString() + std::string(" ]");
}

Spectrum SpecularTransmission::Sample_f(const Vector3f &wo, Vector3f *wi,
                                        const Point2f &sample, float *pdf,
                                        BxDFType *sampledType) const {
    // Figure out which $\eta$ is incident and which is transmitted
    bool entering = CosTheta(wo) > 0;
    float etaI = entering ? etaA : etaB;
    float etaT = entering ? etaB : etaA;

    // Compute ray direction for specular transmission
    if (!Refract(wo, Faceforward(Normal3f(0, 0, 1), wo), etaI / etaT, wi))
        return 0;
    *pdf = 1;
    Spectrum ft = T * (Spectrum(1.) - fresnel.Evaluate(CosTheta(*wi)));
    // Account for non-symmetry with transmission to different medium
    if (mode == TransportMode::Radiance) ft *= (etaI * etaI) / (etaT * etaT);
    return ft / AbsCosTheta(*wi);
}

std::string SpecularTransmission::ToString() const {
    // return std::string("[ SpecularTransmission: T: ") + T.ToString() +
    //        StringPrintf(" etaA: %f etaB: %f ", etaA, etaB) +
    //        std::string(" fresnel: ") + fresnel.ToString() +
    //        std::string(" mode : ") +
    //        (mode == TransportMode::Radiance ? std::string("RADIANCE")
    //                                         : std::string("IMPORTANCE")) +
    //        std::string(" ]");
    
    std::ostringstream ss;
    ss << "[ SpecularTransmission: T: " << T.ToString();
    ss << "  etaA: " << etaA << " etaB: " << etaB;
    ss << " fresenl: " << fresnel.ToString();
    ss << " mode: " << (mode == TransportMode::Radiance ? std::string("RADIANCE")
                                            : std::string("IMPORTANCE"));
    ss << " ]";

    return ss.str();
}

Spectrum FresnelSpecular::Sample_f(const Vector3f &wo, Vector3f *wi,
                                   const Point2f &u, float *pdf,
                                   BxDFType *sampledType) const {
    float F = FrDielectric(CosTheta(wo), etaA, etaB);
    if (u[0] < F) {
        // Compute specular reflection for _FresnelSpecular_

        // Compute perfect specular reflection direction
        *wi = Vector3f(-wo.x, -wo.y, wo.z);
        if (sampledType)
            *sampledType = BxDFType(BSDF_SPECULAR | BSDF_REFLECTION);
        *pdf = F;
        return F * R / AbsCosTheta(*wi);
    } else {
        // Compute specular transmission for _FresnelSpecular_

        // Figure out which $\eta$ is incident and which is transmitted
        bool entering = CosTheta(wo) > 0;
        float etaI = entering ? etaA : etaB;
        float etaT = entering ? etaB : etaA;

        // Compute ray direction for specular transmission
        if (!Refract(wo, Faceforward(Normal3f(0, 0, 1), wo), etaI / etaT, wi))
            return 0;
        Spectrum ft = T * (1 - F);

        // Account for non-symmetry with transmission to different medium
        if (mode == TransportMode::Radiance)
            ft *= (etaI * etaI) / (etaT * etaT);
        if (sampledType)
            *sampledType = BxDFType(BSDF_SPECULAR | BSDF_TRANSMISSION);
        *pdf = 1 - F;
        return ft / AbsCosTheta(*wi);
    }
}

std::string FresnelSpecular::ToString() const {
    // return std::string("[ FresnelSpecular R: ") + R.ToString() +
    //        std::string(" T: ") + T.ToString() +
    //        StringPrintf(" etaA: %f etaB: %f ", etaA, etaB) +
    //        std::string(" mode : ") +
    //        (mode == TransportMode::Radiance ? std::string("RADIANCE")
    //                                         : std::string("IMPORTANCE")) +
    //        std::string(" ]");

    std::ostringstream ss;
    ss << "[ FresnelSpecular R: " << R.ToString();
    ss << " T: " << T.ToString();
    ss << " etaA: " << etaA << " etaB: " << etaB;
    ss << " mode: " << (mode == TransportMode::Radiance ? std::string("RADIANCE")
                                            : std::string("IMPORTANCE"));
    ss << " ]";

    return ss.str();
}

}