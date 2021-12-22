#pragma once

#include "PBRender.h"
#include "material.h"

namespace PBRender {

class MirrorMaterial : public Material {
    public:
        MirrorMaterial(const std::shared_ptr<Texture<Spectrum>> &r,
                       const std::shared_ptr<Texture<float>> &bump) {
            Kr = r;
            bumpMap = bump;
        }

        void ComputeScatteringFunctions(SurfaceInteraction *si, 
                                        // MemoryArena &arena,
                                        TransportMode mode,
                                        bool allowMultipleLobes) const;
    
    private:
        std::shared_ptr<Texture<Spectrum>> Kr;
        std::shared_ptr<Texture<float>> bumpMap;
};

}