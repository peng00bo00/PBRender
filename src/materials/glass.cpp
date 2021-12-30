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
    // si->bsdf = new BSDF(*si);
    si->bsdf = std::make_shared<BSDF>(*si);
    float eta = index->Evaluate(*si);

    Spectrum R = Kr->Evaluate(*si).Clamp();
    Spectrum T = Kt->Evaluate(*si).Clamp();

    if (!R.IsBlack() && !T.IsBlack())
        si->bsdf->Add(std::make_shared<SpecularTransmission>(SpecularTransmission(T, 1.0f, eta, mode)));
}

void GlassMaterial::ComputeScatteringFunctions(SurfaceInteraction *si, 
                                               // MemoryArena &arena,
                                               TransportMode mode,
                                               bool allowMultipleLobes) const {
    // Perform bump mapping with _bumpMap_, if present
    if (bumpMap) Bump(bumpMap, si);
    float eta = index->Evaluate(*si);
    float urough = uRoughness->Evaluate(*si);
    float vrough = vRoughness->Evaluate(*si);
    Spectrum R = Kr->Evaluate(*si).Clamp();
    Spectrum T = Kt->Evaluate(*si).Clamp();

    // Initialize _bsdf_ for smooth or rough dielectric
    si->bsdf = std::make_shared<BSDF>(*si, eta);

    if (R.IsBlack() && T.IsBlack()) return;

    bool isSpecular = urough == 0 && vrough == 0;
    if (isSpecular && allowMultipleLobes) {
        si->bsdf->Add(std::make_shared<FresnelSpecular>(R, T, 1.f, eta, mode));
    } else {
        if (remapRoughness) {
            urough = TrowbridgeReitzDistribution::RoughnessToAlpha(urough);
            vrough = TrowbridgeReitzDistribution::RoughnessToAlpha(vrough);
        }

        std::shared_ptr<MicrofacetDistribution> distrib = 
            isSpecular ? nullptr : std::make_shared<TrowbridgeReitzDistribution>(urough, vrough);
    
        if (!R.IsBlack()) {
            auto fresnel = std::make_shared<FresnelDielectric>(1.f, eta);
            if (isSpecular) si->bsdf->Add(std::make_shared<SpecularReflection>(R, fresnel));
            else si->bsdf->Add(std::make_shared<MicrofacetReflection>(R, distrib, fresnel));
        }

        if (!T.IsBlack()) {
            if (isSpecular)
                si->bsdf->Add(std::make_shared<SpecularTransmission>(T, 1.f, eta, mode));
            else
                si->bsdf->Add(std::make_shared<MicrofacetTransmission>(T, distrib, 1.f, eta, mode));
        }

    }
}

}