#pragma once

#include "PBRender.h"

#include "primitive.h"
#include "spectrum.h"
// #include "light.h"
// #include "reflection.h"
#include "sampler.h"
// #include "material.h"

namespace PBRender {

// Integrator Declarations
class Integrator {
    public:
        // Integrator Interface
        virtual ~Integrator();
        virtual void Render(const Scene &scene) = 0;
        virtual void Test(const Scene &scene, const Vector2f &fullResolution, std::vector<Spectrum> &col) = 0;
};

// SamplerIntegrator Declarations
class SamplerIntegrator : public Integrator {
    public:
        SamplerIntegrator(std::shared_ptr<const Camera> camera,
                          std::shared_ptr<Sampler> sampler,
                          const Bounds2i &pixelBounds)
        : camera(camera), sampler(sampler), pixelBounds(pixelBounds) {}

        void Render(const Scene &scene);
        void Test(const Scene &scene, const Vector2f &fullResolution, std::vector<Spectrum> &col);
    
    protected:
        // SamplerIntegrator Protected Data
        std::shared_ptr<const Camera> camera;
    
    private:
        // SamplerIntegrator Private Data
        std::shared_ptr<Sampler> sampler;
        const Bounds2i pixelBounds;
};

}