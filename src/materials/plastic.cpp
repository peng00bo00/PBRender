#include "materials/plastic.h"
#include "spectrum.h"
#include "reflection.h"
// #include "paramset.h"
#include "texture.h"
#include "interaction.h"

namespace PBRender {

// PlasticMaterial Method Definitions
void PlasticMaterial::ComputeScatteringFunctions(
    SurfaceInteraction *si,
    //    MemoryArena &arena,
    TransportMode mode,
    bool allowMultipleLobes) const {
    // Perform bump mapping with _bumpMap_, if present
    if (bumpMap) Bump(bumpMap, si);

    si->bsdf = std::make_shared<BSDF>(*si);

    // Initialize diffuse component of plastic material
    Spectrum kd = Kd->Evaluate(*si).Clamp();
    if (!kd.IsBlack())
        si->bsdf->Add(std::make_shared<LambertianReflection>(kd));

    // Initialize specular component of plastic material
    Spectrum ks = Ks->Evaluate(*si).Clamp();
    if (!ks.IsBlack()) {
        auto fresnel = std::make_shared<FresnelDielectric>(1.5f, 1.1f);

        // Create microfacet distribution _distrib_ for plastic material
        float rough = roughness->Evaluate(*si);
        if (remapRoughness)
            rough = TrowbridgeReitzDistribution::RoughnessToAlpha(rough);
        
        auto distrib = std::make_shared<TrowbridgeReitzDistribution>(rough, rough);

        si->bsdf->Add(std::make_shared<MicrofacetReflection>(ks, distrib, fresnel));
    }

}

}