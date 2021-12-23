#include "materials/glass.h"
#include "spectrum.h"
#include "reflection.h"
// #include "paramset.h"
#include "texture.h"
#include "interaction.h"

namespace PBRender {

void PerfectGlassMaterial::ComputeScatteringFunctions(SurfaceInteraction *si, 
                                                      // MemoryArena &arena,
                                                      TransportMode mode,
                                                      bool allowMultipleLobes) const {
    si->bsdf = new BSDF(*si);
    float eta = index->Evaluate(*si);

    Spectrum R = Kr->Evaluate(*si).Clamp();
    Spectrum T = Kt->Evaluate(*si).Clamp();

    if (!R.IsBlack() && !T.IsBlack())
        si->bsdf->Add(new SpecularTransmission(T, 1.0f, eta, mode));
}

}