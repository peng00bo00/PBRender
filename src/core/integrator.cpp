#include "integrator.h"
#include "scene.h"
#include "interaction.h"
#include "sampling.h"
// #include "parallel.h"
// #include "film.h"
#include "sampler.h"
// #include "progressreporter.h"
#include "camera.h"
// #include "stats.h"

#include "omp.h"

namespace PBRender {

static long long nCameraRays = 0;

// Integrator Method Definitions
Integrator::~Integrator() {}

// void SamplerIntegrator::Render(const Scene &scene) {}


// current rendering pipeline, to be changed later...
void SamplerIntegrator::Render(const Scene &scene, std::vector<Spectrum> &col) {
    int rasterX = pixelBounds.pMax.x - pixelBounds.pMin.x;
    int rasterY = pixelBounds.pMax.y - pixelBounds.pMin.y;

    omp_set_num_threads(4);
    #pragma omp parallel for collapse(2)
    for (size_t i = 0; i < rasterX; ++i) {
        for (size_t j = 0; j < rasterY; ++j) {
            int offset = (i + rasterX * j);
            std::unique_ptr<Sampler> pixel_sampler = sampler->Clone(offset);

            Point2i pixel(i, j);
            pixel_sampler->StartPixel(pixel);
            Spectrum colObj(0.0f);

            do {
                CameraSample cs = pixel_sampler->GetCameraSample(pixel);

                Ray r;
                camera->GenerateRay(cs, &r);

                colObj += Li(r, scene, *pixel_sampler, 0);
                    
            } while (pixel_sampler->StartNextSample());

            colObj /= (float)pixel_sampler->samplesPerPixel;
            col[offset] = colObj;
        }
        
    }

    std::cout << "Rendering is finished!" << std::endl;

};

Spectrum SamplerIntegrator::SpecularReflect(
    const Ray &ray, const SurfaceInteraction &isect,
    const Scene &scene, Sampler &sampler, 
    // MemoryArena &arena, 
    int depth) const {
    // Compute specular reflection direction _wi_ and BSDF value
    Vector3f wo = isect.wo, wi;
    float pdf;
    BxDFType type = BxDFType(BSDF_REFLECTION | BSDF_SPECULAR);
    Spectrum f = isect.bsdf->Sample_f(wo, &wi, sampler.Get2D(), &pdf, type);

    // Return contribution of specular reflection
    const Normal3f &ns = isect.shading.n;
    if (pdf > 0.f && !f.IsBlack() && AbsDot(wi, ns) != 0.f) {
        // Compute ray differential _rd_ for specular reflection
        // RayDifferential rd = isect.SpawnRay(wi);
        // if (ray.hasDifferentials) {
        //     rd.hasDifferentials = true;
        //     rd.rxOrigin = isect.p + isect.dpdx;
        //     rd.ryOrigin = isect.p + isect.dpdy;
        //     // Compute differential reflected directions
        //     Normal3f dndx = isect.shading.dndu * isect.dudx +
        //                     isect.shading.dndv * isect.dvdx;
        //     Normal3f dndy = isect.shading.dndu * isect.dudy +
        //                     isect.shading.dndv * isect.dvdy;
        //     Vector3f dwodx = -ray.rxDirection - wo,
        //              dwody = -ray.ryDirection - wo;
        //     float dDNdx = Dot(dwodx, ns) + Dot(wo, dndx);
        //     float dDNdy = Dot(dwody, ns) + Dot(wo, dndy);
        //     rd.rxDirection =
        //         wi - dwodx + 2.f * Vector3f(Dot(wo, ns) * dndx + dDNdx * ns);
        //     rd.ryDirection =
        //         wi - dwody + 2.f * Vector3f(Dot(wo, ns) * dndy + dDNdy * ns);
        // }
        // return f * Li(rd, scene, sampler, arena, depth + 1) * AbsDot(wi, ns) /
        //        pdf;
        Ray rd = isect.SpawnRay(wi);
        return f * Li(rd, scene, sampler, depth + 1) * AbsDot(wi, ns) / pdf;
    } else
        return Spectrum(0.f);
}

Spectrum SamplerIntegrator::SpecularTransmit(
    const Ray &ray, const SurfaceInteraction &isect,
    const Scene &scene, Sampler &sampler, 
    // MemoryArena &arena, 
    int depth) const {
    Vector3f wo = isect.wo, wi;
    float pdf;
    const Point3f &p = isect.p;
    const BSDF &bsdf = *isect.bsdf;
    Spectrum f = bsdf.Sample_f(wo, &wi, sampler.Get2D(), &pdf,
                               BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR));
    Spectrum L = Spectrum(0.f);
    Normal3f ns = isect.shading.n;
    if (pdf > 0.f && !f.IsBlack() && AbsDot(wi, ns) != 0.f) {
        // Compute ray differential _rd_ for specular transmission
        Ray rd = isect.SpawnRay(wi);
        // if (ray.hasDifferentials) {
        //     rd.hasDifferentials = true;
        //     rd.rxOrigin = p + isect.dpdx;
        //     rd.ryOrigin = p + isect.dpdy;

        //     Normal3f dndx = isect.shading.dndu * isect.dudx +
        //                     isect.shading.dndv * isect.dvdx;
        //     Normal3f dndy = isect.shading.dndu * isect.dudy +
        //                     isect.shading.dndv * isect.dvdy;

        //     // The BSDF stores the IOR of the interior of the object being
        //     // intersected.  Compute the relative IOR by first out by
        //     // assuming that the ray is entering the object.
        //     float eta = 1 / bsdf.eta;
        //     if (Dot(wo, ns) < 0) {
        //         // If the ray isn't entering, then we need to invert the
        //         // relative IOR and negate the normal and its derivatives.
        //         eta = 1 / eta;
        //         ns = -ns;
        //         dndx = -dndx;
        //         dndy = -dndy;
        //     }

        //     /*
        //       Notes on the derivation:
        //       - pbrt computes the refracted ray as: \wi = -\eta \omega_o + [ \eta (\wo \cdot \N) - \cos \theta_t ] \N
        //         It flips the normal to lie in the same hemisphere as \wo, and then \eta is the relative IOR from
        //         \wo's medium to \wi's medium.
        //       - If we denote the term in brackets by \mu, then we have: \wi = -\eta \omega_o + \mu \N
        //       - Now let's take the partial derivative. (We'll use "d" for \partial in the following for brevity.)
        //         We get: -\eta d\omega_o / dx + \mu dN/dx + d\mu/dx N.
        //       - We have the values of all of these except for d\mu/dx (using bits from the derivation of specularly
        //         reflected ray deifferentials).
        //       - The first term of d\mu/dx is easy: \eta d(\wo \cdot N)/dx. We already have d(\wo \cdot N)/dx.
        //       - The second term takes a little more work. We have:
        //          \cos \theta_i = \sqrt{1 - \eta^2 (1 - (\wo \cdot N)^2)}.
        //          Starting from (\wo \cdot N)^2 and reading outward, we have \cos^2 \theta_o, then \sin^2 \theta_o,
        //          then \sin^2 \theta_i (via Snell's law), then \cos^2 \theta_i and then \cos \theta_i.
        //       - Let's take the partial derivative of the sqrt expression. We get:
        //         1 / 2 * 1 / \cos \theta_i * d/dx (1 - \eta^2 (1 - (\wo \cdot N)^2)).
        //       - That partial derivatve is equal to:
        //         d/dx \eta^2 (\wo \cdot N)^2 = 2 \eta^2 (\wo \cdot N) d/dx (\wo \cdot N).
        //       - Plugging it in, we have d\mu/dx =
        //         \eta d(\wo \cdot N)/dx - (\eta^2 (\wo \cdot N) d/dx (\wo \cdot N))/(-\wi \cdot N).
        //      */
        //     Vector3f dwodx = -ray.rxDirection - wo,
        //              dwody = -ray.ryDirection - wo;
        //     float dDNdx = Dot(dwodx, ns) + Dot(wo, dndx);
        //     float dDNdy = Dot(dwody, ns) + Dot(wo, dndy);

        //     float mu = eta * Dot(wo, ns) - AbsDot(wi, ns);
        //     float dmudx =
        //         (eta - (eta * eta * Dot(wo, ns)) / AbsDot(wi, ns)) * dDNdx;
        //     float dmudy =
        //         (eta - (eta * eta * Dot(wo, ns)) / AbsDot(wi, ns)) * dDNdy;

        //     rd.rxDirection =
        //         wi - eta * dwodx + Vector3f(mu * dndx + dmudx * ns);
        //     rd.ryDirection =
        //         wi - eta * dwody + Vector3f(mu * dndy + dmudy * ns);
        // }
        L = f * Li(rd, scene, sampler, depth + 1) * AbsDot(wi, ns) / pdf;
    }
    return L;
}

}