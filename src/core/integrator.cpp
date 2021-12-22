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
                    VisibilityTester vist;
                    Vector3f wi;
                    Interaction p1;
                    float pdf_light;

                    // sample light
                    Spectrum Li = scene.lights[0]->Sample_Li(isect, 
                                                             pixel_sampler->Get2D(),
                                                             &wi,
                                                             &pdf_light,
                                                             &vist);

                    if (vist.Unoccluded(scene)) {
                        isect.ComputeScatteringFunctions(r);
                        Vector3f wo = isect.wo;

                        Spectrum f = isect.bsdf->f(wo, wi);

                        float pdf_scattering = isect.bsdf->Pdf(wo, wi);

                        colObj += Li * pdf_scattering * f * 3.0f / pdf_light;
                    }


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