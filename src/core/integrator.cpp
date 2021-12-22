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

namespace PBRender {

static long long nCameraRays = 0;

// Integrator Method Definitions
Integrator::~Integrator() {}

// void SamplerIntegrator::Render(const Scene &scene) {}


// current rendering pipeline, to be changed later...
void SamplerIntegrator::Render(const Scene &scene, std::vector<Spectrum> &col) {
    int rasterX = pixelBounds.pMax.x - pixelBounds.pMin.x;
    int rasterY = pixelBounds.pMax.y - pixelBounds.pMin.y;

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

}