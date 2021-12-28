#pragma once

#include "PBRender.h"

#include "primitive.h"
#include "spectrum.h"
#include "light.h"
#include "reflection.h"
#include "sampler.h"
#include "material.h"

namespace PBRender {

// Integrator Declarations
class Integrator {
    public:
        // Integrator Interface
        virtual ~Integrator();
        // virtual void Render(const Scene &scene) = 0;
        virtual void Render(const Scene &scene, std::vector<Spectrum> &col) = 0;
};

Spectrum UniformSampleAllLights(const Interaction &it, 
                                const Scene &scene,
                                // MemoryArena &arena, 
                                Sampler &sampler,
                                const std::vector<int> &nLightSamples,
                                bool handleMedia = false);

Spectrum UniformSampleOneLight(const Interaction &it, 
                               const Scene &scene,
                            //    MemoryArena &arena, 
                               Sampler &sampler,
                               bool handleMedia = false,
                               const Distribution1D *lightDistrib = nullptr);

Spectrum EstimateDirect(const Interaction &it, const Point2f &uShading,
                        const Light &light, const Point2f &uLight,
                        const Scene &scene, Sampler &sampler,
                        // MemoryArena &arena, 
                        bool handleMedia = false,
                        bool specular = false);

// SamplerIntegrator Declarations
class SamplerIntegrator : public Integrator {
    public:
        SamplerIntegrator(std::shared_ptr<const Camera> camera,
                          std::shared_ptr<Sampler> sampler,
                          const Bounds2i &pixelBounds)
        : camera(camera), sampler(sampler), pixelBounds(pixelBounds) {}

        virtual void Preprocess(const Scene &scene, Sampler &sampler) {}

        // void Render(const Scene &scene);
        void Render(const Scene &scene, std::vector<Spectrum> &col);

        virtual Spectrum Li(const Ray &ray, const Scene &scene,
                            Sampler &sampler, 
                            // MemoryArena &arena,
                            int depth = 0) const = 0;

        Spectrum SpecularReflect(const Ray &ray,
                                 const SurfaceInteraction &isect,
                                 const Scene &scene, Sampler &sampler,
                                 //  MemoryArena &arena, 
                                 int depth) const;
        
        Spectrum SpecularTransmit(const Ray &ray,
                                  const SurfaceInteraction &isect,
                                  const Scene &scene, Sampler &sampler,
                                //   MemoryArena &arena, 
                                  int depth) const;

    protected:
        // SamplerIntegrator Protected Data
        std::shared_ptr<const Camera> camera;
    
    private:
        // SamplerIntegrator Private Data
        std::shared_ptr<Sampler> sampler;
        const Bounds2i pixelBounds;
};

}