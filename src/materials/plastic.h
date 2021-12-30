#pragma once

#include "PBRender.h"
#include "material.h"

namespace PBRender {

// PlasticMaterial Declarations
class PlasticMaterial : public Material {
    public:
        PlasticMaterial(const std::shared_ptr<Texture<Spectrum>> &Kd,
                        const std::shared_ptr<Texture<Spectrum>> &Ks,
                        const std::shared_ptr<Texture<float>> &roughness,
                        const std::shared_ptr<Texture<float>> &bumpMap,
                        bool remapRoughness)
            : Kd(Kd),
              Ks(Ks),
              roughness(roughness),
              bumpMap(bumpMap),
              remapRoughness(remapRoughness) {}

        void ComputeScatteringFunctions(SurfaceInteraction *si, 
                                        // MemoryArena &arena,
                                        TransportMode mode,
                                        bool allowMultipleLobes) const;
    
    private:
        // PlasticMaterial Private Data
        std::shared_ptr<Texture<Spectrum>> Kd, Ks;
        std::shared_ptr<Texture<float>> roughness, bumpMap;
        const bool remapRoughness;
};

}