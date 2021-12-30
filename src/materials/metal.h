#pragma once

#include "PBRender.h"
#include "material.h"
#include "spectrum.h"

namespace PBRender {

class MetalMaterial : public Material {
    public:
        MetalMaterial(const std::shared_ptr<Texture<Spectrum>> &eta,
                      const std::shared_ptr<Texture<Spectrum>> &k,
                      const std::shared_ptr<Texture<float>> &rough,
                      const std::shared_ptr<Texture<float>> &urough,
                      const std::shared_ptr<Texture<float>> &vrough,
                      const std::shared_ptr<Texture<float>> &bump,
                      bool remapRoughness);

        void ComputeScatteringFunctions(SurfaceInteraction *si, 
                                        // MemoryArena &arena,
                                        TransportMode mode,
                                        bool allowMultipleLobes) const;
    
    private:
        // MetalMaterial Private Data
        std::shared_ptr<Texture<Spectrum>> eta, k;
        std::shared_ptr<Texture<float>> roughness, uRoughness, vRoughness;
        std::shared_ptr<Texture<float>> bumpMap;
        bool remapRoughness;
};

std::shared_ptr<MetalMaterial> CreateCopperMaterial();

}