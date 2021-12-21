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

void SamplerIntegrator::Render(const Scene &scene) {}


// current rendering pipeline, to be changed later...
void SamplerIntegrator::Test(const Scene &scene, const Vector2f &fullResolution, std::vector<Spectrum> &col) {
    Point3f Light(1.0, 10.0,-10.0);

    for (size_t i = 0; i < int(fullResolution.x); i++)
    {
        for (size_t j = 0; j < int(fullResolution.y); j++)
        {
            int offset = (i + (int)fullResolution.x * j);
            std::unique_ptr<Sampler> pixel_sampler = sampler->Clone(offset);

            Point2i pixel(i, j);
            pixel_sampler->StartPixel(pixel);
            Spectrum colObj(0.0f);

            do {
                CameraSample cs;
                cs = pixel_sampler->GetCameraSample(pixel);

                Ray r;
                camera->GenerateRay(cs, &r);
                SurfaceInteraction isect;

                if (scene.Intersect(r, &isect)) {
                    // scattering
                    isect.ComputeScatteringFunctions(r);
                    Vector3f wo = isect.wo;

                    Vector3f LightNorm = Light - isect.p;
                    LightNorm = Normalize(LightNorm);

                    Vector3f wi = LightNorm;
                    Spectrum f = isect.bsdf->f(wo, wi);
                    float pdf = isect.bsdf->Pdf(wo, wi);

                    colObj += pdf * f * 3.0f;

                    if(isect.bsdf) {
                        delete isect.bsdf;
                        isect.bsdf = nullptr;
                    }
                }
            } while (pixel_sampler->StartNextSample());

            colObj /= (float)pixel_sampler->samplesPerPixel;
            col[offset] = colObj;
        }
        
    }

    std::cout << "Rendering is finished!" << std::endl;

};

}