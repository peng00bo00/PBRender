#pragma once

#include "PBRender.h"
#include "integrator.h"
#include "scene.h"

namespace PBRender {

// LightStrategy Declarations
enum class LightStrategy { UniformSampleAll, UniformSampleOne };

class DirectLightingIntegrator : public SamplerIntegrator {
    public:
        DirectLightingIntegrator(LightStrategy strategy, 
                                 int maxDepth,
                                 std::shared_ptr<const Camera> camera,
                                 std::shared_ptr<Sampler> sampler,
                                 const Bounds2i &pixelBounds)
            : SamplerIntegrator(camera, sampler, pixelBounds),
              strategy(strategy),
              maxDepth(maxDepth) {}
        
        Spectrum Li(const Ray &ray, const Scene &scene,
                    Sampler &sampler, int depth) const;
        
        void Preprocess(const Scene &scene, Sampler &sampler);
    
    private:
        // DirectLightingIntegrator Private Data
        const LightStrategy strategy;
        const int maxDepth;
        std::vector<int> nLightSamples;
};

}