#pragma once

#include "PBRender.h"
#include "material.h"

namespace PBRender {

class PerfectGlassMaterial : public Material {
    public:
        PerfectGlassMaterial(const std::shared_ptr<Texture<Spectrum>> &r,
                             const std::shared_ptr<Texture<Spectrum>> &t,
                             const std::shared_ptr<Texture<float>> &id,
                             const std::shared_ptr<Texture<float>> &bump)
            : Kr(r), Kt(t), index(id), bumpMap(bump) {}

        void ComputeScatteringFunctions(SurfaceInteraction *si, 
                                        // MemoryArena &arena,
                                        TransportMode mode,
                                        bool allowMultipleLobes) const;
    
    private:
        std::shared_ptr<Texture<Spectrum>> Kr, Kt;
        std::shared_ptr<Texture<float>> index;
        std::shared_ptr<Texture<float>> bumpMap;
};

class GlassMaterial : public Material {
    public:
        GlassMaterial(const std::shared_ptr<Texture<Spectrum>> &Kr,
                      const std::shared_ptr<Texture<Spectrum>> &Kt,
                      const std::shared_ptr<Texture<float>> &uRoughness,
                      const std::shared_ptr<Texture<float>> &vRoughness,
                      const std::shared_ptr<Texture<float>> &index,
                      const std::shared_ptr<Texture<float>> &bumpMap,
                      bool remapRoughness)
            : Kr(Kr),
              Kt(Kt),
              uRoughness(uRoughness),
              vRoughness(vRoughness),
              index(index),
              bumpMap(bumpMap),
              remapRoughness(remapRoughness) {}

        void ComputeScatteringFunctions(SurfaceInteraction *si, 
                                        // MemoryArena &arena,
                                        TransportMode mode,
                                        bool allowMultipleLobes) const;
    
    private:
        // GlassMaterial Private Data
        std::shared_ptr<Texture<Spectrum>> Kr, Kt;
        std::shared_ptr<Texture<float>> uRoughness, vRoughness;
        std::shared_ptr<Texture<float>> index;
        std::shared_ptr<Texture<float>> bumpMap;
        bool remapRoughness;
    
};

}