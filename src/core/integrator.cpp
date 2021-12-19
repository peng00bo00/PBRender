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

#include <omp.h>

namespace PBRender {

static long long nCameraRays = 0;

// Integrator Method Definitions
Integrator::~Integrator() {}

void SamplerIntegrator::Render(const Scene &scene) {}


// current rendering pipeline, to be changed later...
void SamplerIntegrator::Test(const Scene &scene, const Vector2f &fullResolution, std::vector<Spectrum> &col) {
    Vector3f Light(10.0, 10.0,-1.0);
    Light = Normalize(Light);

    int NUM_PROCS =  omp_get_num_procs();
    std::cout << "Rendering begins! " << "Using " << NUM_PROCS << " cores." << std::endl;

    omp_set_num_threads(NUM_PROCS);
    #pragma omp parallel for
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
                camera->GenerateRay(cs, &r) ;
                float tHit;
                SurfaceInteraction isect;

                if (scene.Intersect(r, &isect)) {
                    float Li = Dot(Light, isect.n);
                    Li = Clamp(Li, 0.0, 1.0);
                    Li = sqrt(Li);

                    colObj[0] += Li;
                    colObj[1] += Li;
                    colObj[2] += Li;
                }
            } while (pixel_sampler->StartNextSample());

            colObj /= (float)pixel_sampler->samplesPerPixel;
            col[offset] = colObj;
        }
        
    }

    std::cout << "Rendering is finished!" << std::endl;

};

}