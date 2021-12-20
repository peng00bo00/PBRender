#pragma once

#include "PBRender.h"
#include "material.h"

namespace PBRender {

// MatteMaterial Declarations
class MatteMaterial : public Material {
    public:
        // MatteMaterial Public Methods
        MatteMaterial(const std::shared_ptr<Texture<Spectrum>> &Kd,
                      const std::shared_ptr<Texture<float>> &sigma,
                      const std::shared_ptr<Texture<float>> &bumpMap)
            : Kd(Kd), sigma(sigma), bumpMap(bumpMap) {}
        
        void ComputeScatteringFunctions(SurfaceInteraction *si, 
                                        // MemoryArena &arena,
                                        TransportMode mode,
                                        bool allowMultipleLobes) const;

    private:
        // MatteMaterial Private Data
        std::shared_ptr<Texture<Spectrum>> Kd;
        std::shared_ptr<Texture<float>> sigma, bumpMap;
};

// MatteMaterial *CreateMatteMaterial(const TextureParams &mp);

}