#include "materials/mirror.h"
#include "spectrum.h"
#include "reflection.h"
// #include "paramset.h"
#include "texture.h"
#include "interaction.h"

namespace PBRender {

void MirrorMaterial::ComputeScatteringFunctions(SurfaceInteraction *si,
                                                // MemoryArena &arena,
                                                TransportMode mode,
                                                bool allowMultipleLobes) const {
    // Perform bump mapping with _bumpMap_, if present
    if (bumpMap) Bump(bumpMap, si);

    // si->bsdf = ARENA_ALLOC(arena, BSDF)(*si);
    si->bsdf = new BSDF(*si);
    Spectrum R = Kr->Evaluate(*si).Clamp();
    if (!R.IsBlack())
        // si->bsdf->Add(ARENA_ALLOC(arena, SpecularReflection)(
        //     R, ARENA_ALLOC(arena, FresnelNoOp)()));
        si->bsdf->Add(new SpecularReflection(R, new FresnelNoOp()));
}

}