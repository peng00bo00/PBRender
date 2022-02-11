#pragma once

#include "PBRender.h"
#include "integrator.h"
#include "lightdistrib.h"

namespace PBRender {

class PathIntegrator : public SamplerIntegrator {
    public:
        PathIntegrator(int maxDepth, std::shared_ptr<const Camera> camera,
                       std::shared_ptr<Sampler> sampler,
                       const Bounds2i &pixelBounds, float rrThreshold = 1,
                       const std::string &lightSampleStrategy = "uniform");

        void Preprocess(const Scene &scene, Sampler &sampler);
        Spectrum Li(const RayDifferential &ray, const Scene &scene, Sampler &sampler, 
                    // MemoryArena &arena, 
                    int depth) const;

    private:
        const int maxDepth;
        const float rrThreshold;
        const std::string lightSampleStrategy;
        std::unique_ptr<LightDistribution> lightDistribution;
};

}