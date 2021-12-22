#pragma once

#include "PBRender.h"
#include "integrator.h"
#include "scene.h"

namespace PBRender {

class WhittedIntegrator : public SamplerIntegrator {
    public:
        WhittedIntegrator(int maxDepth, 
                          std::shared_ptr<const Camera> camera,
                          std::shared_ptr<Sampler> sampler,
                          const Bounds2i &pixelBounds)
            : SamplerIntegrator(camera, sampler, pixelBounds),
              maxDepth(maxDepth) {}

        Spectrum Li(const Ray &ray, const Scene &scene,
                    Sampler &sampler, int depth) const;
    
    private:
        const int maxDepth;
};

}