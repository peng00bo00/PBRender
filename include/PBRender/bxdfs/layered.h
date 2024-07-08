#pragma once

#include <PBRender/core/bxdf.h>

namespace PBRender
{

// TopOrBottomBxDF Definition
template <typename TopBxDF, typename BottomBxDF>
class TopOrBottomBxDF {
public:
    TopOrBottomBxDF() = default;
    
    TopOrBottomBxDF &operator=(const TopBxDF *t) {
        top = t;
        bottom = nullptr;
        return *this;
    }
    
    TopOrBottomBxDF &operator=(const BottomBxDF *b) {
        bottom = b;
        top = nullptr;
        return *this;
    }
    
    Spectrum f(Vector3f wo, Vector3f wi, TransportMode mode) const {
        return top ? top->f(wo, wi, mode) : bottom->f(wo, wi, mode);
    }

    BSDFSample Sample_f(
        Vector3f wo, float uc, Point2f u, TransportMode mode,
        BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const {
        return top ? top->Sample_f(wo, uc, u, mode, sampleFlags)
                   : bottom->Sample_f(wo, uc, u, mode, sampleFlags);
    }

    float PDF(Vector3f wo, Vector3f wi, TransportMode mode,
              BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const {
        return top ? top->PDF(wo, wi, mode, sampleFlags)
                   : bottom->PDF(wo, wi, mode, sampleFlags);
    }

    BxDFFlags Flags() const { return top ? top->Flags() : bottom->Flags(); }

private:
    const TopBxDF *top = nullptr;
    const BottomBxDF *bottom = nullptr;
};

// // LayeredBxDF Definition (skip for now)
// template <typename TopBxDF, typename BottomBxDF, bool twoSided>
// class LayeredBxDF : public BxDF {
// public:
//     // LayeredBxDF Public Methods
//     LayeredBxDF() = default;
    
//     LayeredBxDF(TopBxDF top, BottomBxDF bottom, float thickness,
//                 const Spectrum &albedo, float g, int maxDepth, int nSamples)
//         : top(top),
//           bottom(bottom),
//           thickness(std::max(thickness, std::numeric_limits<float>::min())),
//           g(g),
//           albedo(albedo),
//           maxDepth(maxDepth),
//           nSamples(nSamples) {}

//     std::string ToString() const;
    
//     void Regularize() {
//         top.Regularize();
//         bottom.Regularize();
//     }

    
//     BxDFFlags Flags() const {
//         BxDFFlags topFlags = top.Flags(), bottomFlags = bottom.Flags();
//         CHECK(IsTransmissive(topFlags) ||
//               IsTransmissive(bottomFlags));  // otherwise, why bother?

//         BxDFFlags flags = BxDFFlags::Reflection;
//         if (IsSpecular(topFlags))
//             flags = flags | BxDFFlags::Specular;

//         if (IsDiffuse(topFlags) || IsDiffuse(bottomFlags) || albedo)
//             flags = flags | BxDFFlags::Diffuse;
//         else if (IsGlossy(topFlags) || IsGlossy(bottomFlags))
//             flags = flags | BxDFFlags::Glossy;

//         if (IsTransmissive(topFlags) && IsTransmissive(bottomFlags))
//             flags = flags | BxDFFlags::Transmission;

//         return flags;
//     }

    
//     Spectrum f(Vector3f wo, Vector3f wi, TransportMode mode) const {
//         Spectrum f(0.);
//         // Estimate _LayeredBxDF_ value _f_ using random sampling
//         // Set _wo_ and _wi_ for layered BSDF evaluation
//         if (twoSided && wo.z < 0) {
//             wo = -wo;
//             wi = -wi;
//         }

//         // Determine entrance interface for layered BSDF
//         TopOrBottomBxDF<TopBxDF, BottomBxDF> enterInterface;
//         bool enteredTop = twoSided || wo.z > 0;
//         if (enteredTop)
//             enterInterface = &top;
//         else
//             enterInterface = &bottom;

//         // Determine exit interface and exit $z$ for layered BSDF
//         TopOrBottomBxDF<TopBxDF, BottomBxDF> exitInterface, nonExitInterface;
//         if (SameHemisphere(wo, wi) ^ enteredTop) {
//             exitInterface = &bottom;
//             nonExitInterface = &top;
//         } else {
//             exitInterface = &top;
//             nonExitInterface = &bottom;
//         }
//         float exitZ = (SameHemisphere(wo, wi) ^ enteredTop) ? 0 : thickness;

//         // Account for reflection at the entrance interface
//         if (SameHemisphere(wo, wi))
//             f = nSamples * enterInterface.f(wo, wi, mode);

//         // Declare _RNG_ for layered BSDF evaluation
//         RNG rng(Hash(GetOptions().seed, wo), Hash(wi));
//         auto r = [&rng]() {
//             return std::min<float>(rng.Uniform<float>(), OneMinusEpsilon);
//         };

//         for (int s = 0; s < nSamples; ++s) {
//             // Sample random walk through layers to estimate BSDF value
//             // Sample transmission direction through entrance interface
//             float uc = r();
//             pstd::optional<BSDFSample> wos = enterInterface.Sample_f(
//                 wo, uc, Point2f(r(), r()), mode, BxDFReflTransFlags::Transmission);
//             if (!wos || !wos->f || wos->pdf == 0 || wos->wi.z == 0)
//                 continue;

//             // Sample BSDF for virtual light from _wi_
//             uc = r();
//             pstd::optional<BSDFSample> wis = exitInterface.Sample_f(
//                 wi, uc, Point2f(r(), r()), !mode, BxDFReflTransFlags::Transmission);
//             if (!wis || !wis->f || wis->pdf == 0 || wis->wi.z == 0)
//                 continue;

//             // Declare state for random walk through BSDF layers
//             Spectrum beta = wos->f * AbsCosTheta(wos->wi) / wos->pdf;
//             float z = enteredTop ? thickness : 0;
//             Vector3f w = wos->wi;
//             HGPhaseFunction phase(g);

//             for (int depth = 0; depth < maxDepth; ++depth) {
//                 // Sample next event for layered BSDF evaluation random walk
//                 PBRT_DBG("beta: %f %f %f %f, w: %f %f %f, f: %f %f %f %f\n", beta[0],
//                          beta[1], beta[2], beta[3], w.x, w.y, w.z, f[0], f[1], f[2],
//                          f[3]);
//                 // Possibly terminate layered BSDF random walk with Russian roulette
//                 if (depth > 3 && beta.MaxComponentValue() < 0.25f) {
//                     float q = std::max<float>(0, 1 - beta.MaxComponentValue());
//                     if (r() < q)
//                         break;
//                     beta /= 1 - q;
//                     PBRT_DBG("After RR with q = %f, beta: %f %f %f %f\n", q, beta[0],
//                              beta[1], beta[2], beta[3]);
//                 }

//                 // Account for media between layers and possibly scatter
//                 if (!albedo) {
//                     // Advance to next layer boundary and update _beta_ for transmittance
//                     z = (z == thickness) ? 0 : thickness;
//                     beta *= Tr(thickness, w);

//                 } else {
//                     // Sample medium scattering for layered BSDF evaluation
//                     float sigma_t = 1;
//                     float dz = SampleExponential(r(), sigma_t / std::abs(w.z));
//                     float zp = w.z > 0 ? (z + dz) : (z - dz);
//                     DCHECK_RARE(1e-5, z == zp);
//                     if (z == zp)
//                         continue;
//                     if (0 < zp && zp < thickness) {
//                         // Handle scattering event in layered BSDF medium
//                         // Account for scattering through _exitInterface_ using _wis_
//                         float wt = 1;
//                         if (!IsSpecular(exitInterface.Flags()))
//                             wt = PowerHeuristic(1, wis->pdf, 1, phase.PDF(-w, -wis->wi));
//                         f += beta * albedo * phase.p(-w, -wis->wi) * wt *
//                              Tr(zp - exitZ, wis->wi) * wis->f / wis->pdf;

//                         // Sample phase function and update layered path state
//                         Point2f u{r(), r()};
//                         pstd::optional<PhaseFunctionSample> ps = phase.Sample_p(-w, u);
//                         if (!ps || ps->pdf == 0 || ps->wi.z == 0)
//                             continue;
//                         beta *= albedo * ps->p / ps->pdf;
//                         w = ps->wi;
//                         z = zp;

//                         // Possibly account for scattering through _exitInterface_
//                         if (((z < exitZ && w.z > 0) || (z > exitZ && w.z < 0)) &&
//                             !IsSpecular(exitInterface.Flags())) {
//                             // Account for scattering through _exitInterface_
//                             Spectrum fExit = exitInterface.f(-w, wi, mode);
//                             if (fExit) {
//                                 float exitPDF = exitInterface.PDF(
//                                     -w, wi, mode, BxDFReflTransFlags::Transmission);
//                                 float wt = PowerHeuristic(1, ps->pdf, 1, exitPDF);
//                                 f += beta * Tr(zp - exitZ, ps->wi) * fExit * wt;
//                             }
//                         }

//                         continue;
//                     }
//                     z = Clamp(zp, 0, thickness);
//                 }

//                 // Account for scattering at appropriate interface
//                 if (z == exitZ) {
//                     // Account for reflection at _exitInterface_
//                     float uc = r();
//                     pstd::optional<BSDFSample> bs = exitInterface.Sample_f(
//                         -w, uc, Point2f(r(), r()), mode, BxDFReflTransFlags::Reflection);
//                     if (!bs || !bs->f || bs->pdf == 0 || bs->wi.z == 0)
//                         break;
//                     beta *= bs->f * AbsCosTheta(bs->wi) / bs->pdf;
//                     w = bs->wi;

//                 } else {
//                     // Account for scattering at _nonExitInterface_
//                     if (!IsSpecular(nonExitInterface.Flags())) {
//                         // Add NEE contribution along presampled _wis_ direction
//                         float wt = 1;
//                         if (!IsSpecular(exitInterface.Flags()))
//                             wt = PowerHeuristic(1, wis->pdf, 1,
//                                                 nonExitInterface.PDF(-w, -wis->wi, mode));
//                         f += beta * nonExitInterface.f(-w, -wis->wi, mode) *
//                              AbsCosTheta(wis->wi) * wt * Tr(thickness, wis->wi) * wis->f /
//                              wis->pdf;
//                     }
//                     // Sample new direction using BSDF at _nonExitInterface_
//                     float uc = r();
//                     Point2f u(r(), r());
//                     pstd::optional<BSDFSample> bs = nonExitInterface.Sample_f(
//                         -w, uc, u, mode, BxDFReflTransFlags::Reflection);
//                     if (!bs || !bs->f || bs->pdf == 0 || bs->wi.z == 0)
//                         break;
//                     beta *= bs->f * AbsCosTheta(bs->wi) / bs->pdf;
//                     w = bs->wi;

//                     if (!IsSpecular(exitInterface.Flags())) {
//                         // Add NEE contribution along direction from BSDF sample
//                         Spectrum fExit = exitInterface.f(-w, wi, mode);
//                         if (fExit) {
//                             float wt = 1;
//                             if (!IsSpecular(nonExitInterface.Flags())) {
//                                 float exitPDF = exitInterface.PDF(
//                                     -w, wi, mode, BxDFReflTransFlags::Transmission);
//                                 wt = PowerHeuristic(1, bs->pdf, 1, exitPDF);
//                             }
//                             f += beta * Tr(thickness, bs->wi) * fExit * wt;
//                         }
//                     }
//                 }
//             }
//         }

//         return f / nSamples;
//     }

    
//     pstd::optional<BSDFSample> Sample_f(
//         Vector3f wo, float uc, Point2f u, TransportMode mode,
//         BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const {
//         CHECK(sampleFlags == BxDFReflTransFlags::All);  // for now
//         // Set _wo_ for layered BSDF sampling
//         bool flipWi = false;
//         if (twoSided && wo.z < 0) {
//             wo = -wo;
//             flipWi = true;
//         }

//         // Sample BSDF at entrance interface to get initial direction _w_
//         bool enteredTop = twoSided || wo.z > 0;
//         pstd::optional<BSDFSample> bs =
//             enteredTop ? top.Sample_f(wo, uc, u, mode) : bottom.Sample_f(wo, uc, u, mode);
//         if (!bs || !bs->f || bs->pdf == 0 || bs->wi.z == 0)
//             return {};
//         if (bs->IsReflection()) {
//             if (flipWi)
//                 bs->wi = -bs->wi;
//             bs->pdfIsProportional = true;
//             return bs;
//         }
//         Vector3f w = bs->wi;
//         bool specularPath = bs->IsSpecular();

//         // Declare _RNG_ for layered BSDF sampling
//         RNG rng(Hash(GetOptions().seed, wo), Hash(uc, u));
//         auto r = [&rng]() {
//             return std::min<float>(rng.Uniform<float>(), OneMinusEpsilon);
//         };

//         // Declare common variables for layered BSDF sampling
//         Spectrum f = bs->f * AbsCosTheta(bs->wi);
//         float pdf = bs->pdf;
//         float z = enteredTop ? thickness : 0;
//         HGPhaseFunction phase(g);

//         for (int depth = 0; depth < maxDepth; ++depth) {
//             // Follow random walk through layers to sample layered BSDF
//             // Possibly terminate layered BSDF sampling with Russian Roulette
//             float rrBeta = f.MaxComponentValue() / pdf;
//             if (depth > 3 && rrBeta < 0.25f) {
//                 float q = std::max<float>(0, 1 - rrBeta);
//                 if (r() < q)
//                     return {};
//                 pdf *= 1 - q;
//             }
//             if (w.z == 0)
//                 return {};

//             if (albedo) {
//                 // Sample potential scattering event in layered medium
//                 float sigma_t = 1;
//                 float dz = SampleExponential(r(), sigma_t / AbsCosTheta(w));
//                 float zp = w.z > 0 ? (z + dz) : (z - dz);
//                 CHECK_RARE(1e-5, zp == z);
//                 if (zp == z)
//                     return {};
//                 if (0 < zp && zp < thickness) {
//                     // Update path state for valid scattering event between interfaces
//                     pstd::optional<PhaseFunctionSample> ps =
//                         phase.Sample_p(-w, Point2f(r(), r()));
//                     if (!ps || ps->pdf == 0 || ps->wi.z == 0)
//                         return {};
//                     f *= albedo * ps->p;
//                     pdf *= ps->pdf;
//                     specularPath = false;
//                     w = ps->wi;
//                     z = zp;

//                     continue;
//                 }
//                 z = Clamp(zp, 0, thickness);
//                 if (z == 0)
//                     DCHECK_LT(w.z, 0);
//                 else
//                     DCHECK_GT(w.z, 0);

//             } else {
//                 // Advance to the other layer interface
//                 z = (z == thickness) ? 0 : thickness;
//                 f *= Tr(thickness, w);
//             }
//             // Initialize _interface_ for current interface surface
// #ifdef interface  // That's enough out of you, Windows.
// #undef interface
// #endif
//             TopOrBottomBxDF<TopBxDF, BottomBxDF> interface;
//             if (z == 0)
//                 interface = &bottom;
//             else
//                 interface = &top;

//             // Sample interface BSDF to determine new path direction
//             float uc = r();
//             Point2f u(r(), r());
//             pstd::optional<BSDFSample> bs = interface.Sample_f(-w, uc, u, mode);
//             if (!bs || !bs->f || bs->pdf == 0 || bs->wi.z == 0)
//                 return {};
//             f *= bs->f;
//             pdf *= bs->pdf;
//             specularPath &= bs->IsSpecular();
//             w = bs->wi;

//             // Return _BSDFSample_ if path has left the layers
//             if (bs->IsTransmission()) {
//                 BxDFFlags flags = SameHemisphere(wo, w) ? BxDFFlags::Reflection
//                                                         : BxDFFlags::Transmission;
//                 flags |= specularPath ? BxDFFlags::Specular : BxDFFlags::Glossy;
//                 if (flipWi)
//                     w = -w;
//                 return BSDFSample(f, w, pdf, flags, 1.f, true);
//             }

//             // Scale _f_ by cosine term after scattering at the interface
//             f *= AbsCosTheta(bs->wi);
//         }
//         return {};
//     }
    
//     float PDF(Vector3f wo, Vector3f wi, TransportMode mode,
//               BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const {
//         CHECK(sampleFlags == BxDFReflTransFlags::All);  // for now
//         // Set _wo_ and _wi_ for layered BSDF evaluation
//         if (twoSided && wo.z < 0) {
//             wo = -wo;
//             wi = -wi;
//         }

//         // Declare _RNG_ for layered PDF evaluation
//         RNG rng(Hash(GetOptions().seed, wi), Hash(wo));
//         auto r = [&rng]() {
//             return std::min<float>(rng.Uniform<float>(), OneMinusEpsilon);
//         };

//         // Update _pdfSum_ for reflection at the entrance layer
//         bool enteredTop = twoSided || wo.z > 0;
//         float pdfSum = 0;
//         if (SameHemisphere(wo, wi)) {
//             auto reflFlag = BxDFReflTransFlags::Reflection;
//             pdfSum += enteredTop ? nSamples * top.PDF(wo, wi, mode, reflFlag)
//                                  : nSamples * bottom.PDF(wo, wi, mode, reflFlag);
//         }

//         for (int s = 0; s < nSamples; ++s) {
//             // Evaluate layered BSDF PDF sample
//             if (SameHemisphere(wo, wi)) {
//                 // Evaluate TRT term for PDF estimate
//                 TopOrBottomBxDF<TopBxDF, BottomBxDF> rInterface, tInterface;
//                 if (enteredTop) {
//                     rInterface = &bottom;
//                     tInterface = &top;
//                 } else {
//                     rInterface = &top;
//                     tInterface = &bottom;
//                 }
//                 // Sample _tInterface_ to get direction into the layers
//                 auto trans = BxDFReflTransFlags::Transmission;
//                 pstd::optional<BSDFSample> wos, wis;
//                 wos = tInterface.Sample_f(wo, r(), {r(), r()}, mode, trans);
//                 wis = tInterface.Sample_f(wi, r(), {r(), r()}, !mode, trans);

//                 // Update _pdfSum_ accounting for TRT scattering events
//                 if (wos && wos->f && wos->pdf > 0 && wis && wis->f && wis->pdf > 0) {
//                     if (!IsNonSpecular(tInterface.Flags()))
//                         pdfSum += rInterface.PDF(-wos->wi, -wis->wi, mode);
//                     else {
//                         // Use multiple importance sampling to estimate PDF product
//                         pstd::optional<BSDFSample> rs =
//                             rInterface.Sample_f(-wos->wi, r(), {r(), r()}, mode);
//                         if (rs && rs->f && rs->pdf > 0) {
//                             if (!IsNonSpecular(rInterface.Flags()))
//                                 pdfSum += tInterface.PDF(-rs->wi, wi, mode);
//                             else {
//                                 // Compute MIS-weighted estimate of Equation
//                                 // (\ref{eq:pdf-triple-canceled-one})
//                                 float rPDF = rInterface.PDF(-wos->wi, -wis->wi, mode);
//                                 float wt = PowerHeuristic(1, wis->pdf, 1, rPDF);
//                                 pdfSum += wt * rPDF;

//                                 float tPDF = tInterface.PDF(-rs->wi, wi, mode);
//                                 wt = PowerHeuristic(1, rs->pdf, 1, tPDF);
//                                 pdfSum += wt * tPDF;
//                             }
//                         }
//                     }
//                 }

//             } else {
//                 // Evaluate TT term for PDF estimate
//                 TopOrBottomBxDF<TopBxDF, BottomBxDF> toInterface, tiInterface;
//                 if (enteredTop) {
//                     toInterface = &top;
//                     tiInterface = &bottom;
//                 } else {
//                     toInterface = &bottom;
//                     tiInterface = &top;
//                 }

//                 float uc = r();
//                 Point2f u(r(), r());
//                 pstd::optional<BSDFSample> wos = toInterface.Sample_f(wo, uc, u, mode);
//                 if (!wos || !wos->f || wos->pdf == 0 || wos->wi.z == 0 ||
//                     wos->IsReflection())
//                     continue;

//                 uc = r();
//                 u = Point2f(r(), r());
//                 pstd::optional<BSDFSample> wis = tiInterface.Sample_f(wi, uc, u, !mode);
//                 if (!wis || !wis->f || wis->pdf == 0 || wis->wi.z == 0 ||
//                     wis->IsReflection())
//                     continue;

//                 if (IsSpecular(toInterface.Flags()))
//                     pdfSum += tiInterface.PDF(-wos->wi, wi, mode);
//                 else if (IsSpecular(tiInterface.Flags()))
//                     pdfSum += toInterface.PDF(wo, -wis->wi, mode);
//                 else
//                     pdfSum += (toInterface.PDF(wo, -wis->wi, mode) +
//                                tiInterface.PDF(-wos->wi, wi, mode)) /
//                               2;
//             }
//         }
//         // Return mixture of PDF estimate and constant PDF
//         return Lerp(0.9f, 1 / (4 * Pi), pdfSum / nSamples);
//     }

// private:
//     // LayeredBxDF Private Methods
//     static float Tr(float dz, Vector3f w) {
//         if (std::abs(dz) <= std::numeric_limits<float>::min())
//             return 1;
//         return FastExp(-std::abs(dz / w.z));
//     }

//     // LayeredBxDF Private Members
//     TopBxDF top;
//     BottomBxDF bottom;
//     float thickness, g;
//     Spectrum albedo;
//     int maxDepth, nSamples;
// };



} // namespace PBRender