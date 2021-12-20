#pragma once

#include "PBRender.h"

namespace PBRender {
// TransportMode Declarations
enum class TransportMode { Radiance, Importance };

// Material Declarations
class Material {
  public:
    // Material Interface
    virtual void ComputeScatteringFunctions(SurfaceInteraction *si,
                                            // MemoryArena &arena,
                                            TransportMode mode,
                                            bool allowMultipleLobes) const = 0;
    virtual ~Material();
    static void Bump(const std::shared_ptr<Texture<float>> &d,
                     SurfaceInteraction *si);
};

}