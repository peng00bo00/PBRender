#include "integrators/path.h"
// #include "bssrdf.h"
#include "camera.h"
// #include "film.h"
#include "interaction.h"
// #include "paramset.h"
#include "scene.h"
// #include "stats.h"

namespace PBRender {

static float zeroRadiancePaths = 0;
static float totalPaths        = 0;
static long long pathLength    = 0;

// PathIntegrator Method Definitions
PathIntegrator::PathIntegrator(int maxDepth,
                               std::shared_ptr<const Camera> camera,
                               std::shared_ptr<Sampler> sampler,
                               const Bounds2i &pixelBounds, float rrThreshold,
                               const std::string &lightSampleStrategy)
    : SamplerIntegrator(camera, sampler, pixelBounds),
      maxDepth(maxDepth),
      rrThreshold(rrThreshold),
      lightSampleStrategy(lightSampleStrategy) {}

void PathIntegrator::Preprocess(const Scene &scene, Sampler &sampler) {
    lightDistribution =
        CreateLightSampleDistribution(lightSampleStrategy, scene);
}

Spectrum PathIntegrator::Li(const RayDifferential &r, const Scene &scene, Sampler &sampler, 
                            // MemoryArena &arena, 
                            int depth) const {
    Spectrum L(0.f), beta(1.f);
    RayDifferential ray(r);
    bool specularBounce = false;
    int bounces;

    float etaScale = 1;

    for (bounces = 0;; ++bounces) {
        // Find next path vertex and accumulate contribution
        // std::cout << "Path tracer bounce " << bounces << ", current L = " << L
        //           << ", beta = " << beta << std::endl;
        
        // Intersect _ray_ with scene and store intersection in _isect_
        SurfaceInteraction isect;
        bool foundIntersection = scene.Intersect(ray, &isect);

        // Possibly add emitted light at intersection
        if (bounces == 0 || specularBounce) {
            // Add emitted light at path vertex or from the environment
            if (foundIntersection) {
                L += beta * isect.Le(-ray.d);
                // std::cout << "Added Le -> L = " << L << std::endl;
            } else {
                for (const auto &light : scene.infiniteLights)
                    L += beta * light->Le(ray);
                // std::cout << "Added infinite area lights -> L = " << L << std::endl;
            }
        }

        // Terminate path if ray escaped or _maxDepth_ was reached
        if (!foundIntersection || bounces >= maxDepth) break;

        // Compute scattering functions and skip over medium boundaries
        isect.ComputeScatteringFunctions(ray, true);
        if (!isect.bsdf) {
            // std::cout << "Skipping intersection due to null bsdf" << std::endl;
            ray = isect.SpawnRay(ray.d);
            bounces--;
            continue;
        }

        const Distribution1D *distrib = lightDistribution->Lookup(isect.p);

        // Sample illumination from lights to find path contribution.
        // (But skip this for perfectly specular BSDFs.)
        if (isect.bsdf->NumComponents(BxDFType(BSDF_ALL & ~BSDF_SPECULAR)) > 0) {
            ++totalPaths;
            Spectrum Ld = beta * UniformSampleOneLight(isect, scene, 
                                                    //    arena,
                                                       sampler, false, distrib);
            if (Ld.IsBlack()) ++zeroRadiancePaths;
            // CHECK_GE(Ld.y(), 0.f);
            assert(Ld.y() >= 0.0f);
            L += Ld;
        }

        // Sample BSDF to get new path direction
        Vector3f wo = -ray.d, wi;
        float pdf;
        BxDFType flags;
        Spectrum f = isect.bsdf->Sample_f(wo, &wi, sampler.Get2D(), &pdf,
                                          BSDF_ALL, &flags);
        if (f.IsBlack() || pdf == 0.f) break;
        beta *= f * AbsDot(wi, isect.shading.n) / pdf;
        // std::cout << "Updated beta = " << beta << std::endl;
        // CHECK_GE(beta.y(), 0.f);
        // DCHECK(!std::isinf(beta.y()));

        assert(beta.y() > 0.f);
        assert(!std::isinf(beta.y()));

        specularBounce = (flags & BSDF_SPECULAR) != 0;
        if ((flags & BSDF_SPECULAR) && (flags & BSDF_TRANSMISSION)) {
            float eta = isect.bsdf->eta;
            // Update the term that tracks radiance scaling for refraction
            // depending on whether the ray is entering or leaving the
            // medium.
            etaScale *= (Dot(wo, isect.n) > 0) ? (eta * eta) : 1 / (eta * eta);
        }
        ray = isect.SpawnRay(wi);

        // // Account for subsurface scattering, if applicable
        // if (isect.bssrdf && (flags & BSDF_TRANSMISSION)) {
        //     // Importance sample the BSSRDF
        //     SurfaceInteraction pi;
        //     Spectrum S = isect.bssrdf->Sample_S(
        //         scene, sampler.Get1D(), sampler.Get2D(), arena, &pi, &pdf);
        //     assert(!std::isinf(beta.y()));
        //     if (S.IsBlack() || pdf == 0) break;
        //     beta *= S / pdf;

        //     // Account for the direct subsurface scattering component
        //     L += beta * UniformSampleOneLight(pi, scene, arena, sampler, false,
        //                                       lightDistribution->Lookup(pi.p));

        //     // Account for the indirect subsurface scattering component
        //     Spectrum f = pi.bsdf->Sample_f(pi.wo, &wi, sampler.Get2D(), &pdf,
        //                                    BSDF_ALL, &flags);
        //     if (f.IsBlack() || pdf == 0) break;
        //     beta *= f * AbsDot(wi, pi.shading.n) / pdf;
        //     assert(!std::isinf(beta.y()));
        //     specularBounce = (flags & BSDF_SPECULAR) != 0;
        //     ray = pi.SpawnRay(wi);
        // }

        // Possibly terminate the path with Russian roulette.
        // Factor out radiance scaling due to refraction in rrBeta.
        Spectrum rrBeta = beta * etaScale;
        if (rrBeta.MaxComponentValue() < rrThreshold && bounces > 3) {
            float q = std::max((float).05, 1 - rrBeta.MaxComponentValue());
            if (sampler.Get1D() < q) break;
            beta /= 1 - q;
            assert(!std::isinf(beta.y()));
        }
    }

    return L;
}

}