#pragma once

#include <PBRender/core/common.h>
#include <PBRender/core/math.h>
#include <PBRender/core/vecmath.h>
#include <PBRender/core/sampling.h>
#include <PBRender/core/spectrum.h>

namespace PBRender
{

inline Vector3f Reflect(Vector3f wo, Vector3f n) {
    return -wo + 2 * Dot(wo, n) * n;
}

inline bool Refract(Vector3f wi, Normal3f n, float eta, float *etap,
                                 Vector3f *wt) {
    float cosTheta_i = Dot(n, wi);
    // Potentially flip interface orientation for Snell's law
    if (cosTheta_i < 0) {
        eta = 1 / eta;
        cosTheta_i = -cosTheta_i;
        n = -n;
    }

    // Compute $\cos\,\theta_\roman{t}$ using Snell's law
    float sin2Theta_i = std::max<float>(0, 1 - Sqr(cosTheta_i));
    float sin2Theta_t = sin2Theta_i / Sqr(eta);
    // Handle total internal reflection case
    if (sin2Theta_t >= 1)
        return false;

    float cosTheta_t = std::sqrt(1 - sin2Theta_t);

    *wt = -wi / eta + (cosTheta_i / eta - cosTheta_t) * Vector3f(n);
    // Provide relative IOR along ray to caller
    if (etap)
        *etap = eta;

    return true;
}

// Fresnel Inline Functions
inline float FrDielectric(float cosTheta_i, float eta) {
    cosTheta_i = Clamp(cosTheta_i, -1, 1);
    // Potentially flip interface orientation for Fresnel equations
    if (cosTheta_i < 0) {
        eta = 1 / eta;
        cosTheta_i = -cosTheta_i;
    }

    // Compute $\cos\,\theta_\roman{t}$ for Fresnel equations using Snell's law
    float sin2Theta_i = 1 - Sqr(cosTheta_i);
    float sin2Theta_t = sin2Theta_i / Sqr(eta);
    if (sin2Theta_t >= 1)
        return 1.f;
    float cosTheta_t = SafeSqrt(1 - sin2Theta_t);

    float r_parl = (eta * cosTheta_i - cosTheta_t) / (eta * cosTheta_i + cosTheta_t);
    float r_perp = (cosTheta_i - eta * cosTheta_t) / (cosTheta_i + eta * cosTheta_t);
    return (Sqr(r_parl) + Sqr(r_perp)) / 2;
}

inline float FrComplex(float cosTheta_i, PBRender::complex<float> eta) {
    using Complex = PBRender::complex<float>;
    cosTheta_i = Clamp(cosTheta_i, 0, 1);
    // Compute complex $\cos\,\theta_\roman{t}$ for Fresnel equations using Snell's law
    float sin2Theta_i = 1 - Sqr(cosTheta_i);
    Complex sin2Theta_t = sin2Theta_i / Sqr(eta);
    Complex cosTheta_t = PBRender::sqrt(1 - sin2Theta_t);

    Complex r_parl = (eta * cosTheta_i - cosTheta_t) / (eta * cosTheta_i + cosTheta_t);
    Complex r_perp = (cosTheta_i - eta * cosTheta_t) / (cosTheta_i + eta * cosTheta_t);
    return (PBRender::norm(r_parl) + PBRender::norm(r_perp)) / 2;
}

inline Spectrum FrComplex(float cosTheta_i, Spectrum eta, Spectrum k) {
    Spectrum result;
    // hardcode nSpectrumSamples=3 here
    // for (int i = 0; i < nSpectrumSamples; ++i)
    for (int i = 0; i < 3; ++i)
        result[i] = FrComplex(cosTheta_i, PBRender::complex<float>(eta[i], k[i]));
    return result;
}

// BSSRDF Utility Declarations
float FresnelMoment1(float invEta);
float FresnelMoment2(float invEta);

// TrowbridgeReitzDistribution Definition
class TrowbridgeReitzDistribution {
public:
    // TrowbridgeReitzDistribution Public Methods
    TrowbridgeReitzDistribution() = default;
    
    TrowbridgeReitzDistribution(float ax, float ay)
        : alpha_x(ax), alpha_y(ay) {
        if (!EffectivelySmooth()) {
            // If one direction has some roughness, then the other can't
            // have zero (or very low) roughness; the computation of |e| in
            // D() blows up in that case.
            alpha_x = std::max<float>(alpha_x, 1e-4f);
            alpha_y = std::max<float>(alpha_y, 1e-4f);
        }
    }

    inline float D(Vector3f wm) const {
        float tan2Theta = Tan2Theta(wm);
        if (std::isinf(tan2Theta))
            return 0;
        float cos4Theta = Sqr(Cos2Theta(wm));
        if (cos4Theta < 1e-16f)
            return 0;
        float e = tan2Theta * (Sqr(CosPhi(wm) / alpha_x) + Sqr(SinPhi(wm) / alpha_y));
        return 1 / (Pi * alpha_x * alpha_y * cos4Theta * Sqr(1 + e));
    }

    bool EffectivelySmooth() const { return std::max(alpha_x, alpha_y) < 1e-3f; }
    
    float G1(Vector3f w) const { return 1 / (1 + Lambda(w)); }

    float Lambda(Vector3f w) const {
        float tan2Theta = Tan2Theta(w);
        if (std::isinf(tan2Theta))
            return 0;
        float alpha2 = Sqr(CosPhi(w) * alpha_x) + Sqr(SinPhi(w) * alpha_y);
        return (std::sqrt(1 + alpha2 * tan2Theta) - 1) / 2;
    }
    
    float G(Vector3f wo, Vector3f wi) const { return 1 / (1 + Lambda(wo) + Lambda(wi)); }
    
    float D(Vector3f w, Vector3f wm) const {
        return G1(w) / AbsCosTheta(w) * D(wm) * AbsDot(w, wm);
    }

    float PDF(Vector3f w, Vector3f wm) const { return D(w, wm); }

    Vector3f Sample_wm(Vector3f w, Point2f u) const {
        // Transform _w_ to hemispherical configuration
        Vector3f wh = Normalize(Vector3f(alpha_x * w.x, alpha_y * w.y, w.z));
        if (wh.z < 0)
            wh = -wh;

        // Find orthonormal basis for visible normal sampling
        Vector3f T1 = (wh.z < 0.99999f) ? Normalize(Cross(Vector3f(0, 0, 1), wh))
                                        : Vector3f(1, 0, 0);
        Vector3f T2 = Cross(wh, T1);

        // Generate uniformly distributed points on the unit disk
        Point2f p = SampleUniformDiskPolar(u);

        // Warp hemispherical projection for visible normal sampling
        float h = std::sqrt(1 - Sqr(p.x));
        p.y = Lerp((1 + wh.z) / 2, h, p.y);

        // Reproject to hemisphere and transform normal to ellipsoid configuration
        float pz = std::sqrt(std::max<float>(0, 1 - Vector2f(p).LengthSquared()));
        Vector3f nh = p.x * T1 + p.y * T2 + pz * wh;
        // CHECK_RARE(1e-5f, nh.z == 0);
        return Normalize(
            Vector3f(alpha_x * nh.x, alpha_y * nh.y, std::max<float>(1e-6f, nh.z)));
    }

    std::string ToString() const;

    
    static float RoughnessToAlpha(float roughness) { return std::sqrt(roughness); }

    void Regularize() {
        if (alpha_x < 0.3f)
            alpha_x = Clamp(2 * alpha_x, 0.1f, 0.3f);
        if (alpha_y < 0.3f)
            alpha_y = Clamp(2 * alpha_y, 0.1f, 0.3f);
    }

private:
    // TrowbridgeReitzDistribution Private Members
    float alpha_x, alpha_y;
};

} // namespace PBRender