#pragma once

#include <PBRender/core/material.h>
#include <PBRender/bxdfs/diffuse.h>

namespace PBRender
{

class DiffuseMaterial : public Material {
  public:
    // DiffuseMaterial Type Definitions
    using BxDF = DiffuseBxDF;
    // using BSSRDF = void;

    // // DiffuseMaterial Public Methods
    // static const char *Name() { return "DiffuseMaterial"; }

    // PBRT_CPU_GPU
    // FloatTexture GetDisplacement() const { return displacement; }
    // PBRT_CPU_GPU
    // const Image *GetNormalMap() const { return normalMap; }

    // static DiffuseMaterial *Create(const TextureParameterDictionary &parameters,
    //                                Image *normalMap, const FileLoc *loc, Allocator alloc);

    // template <typename TextureEvaluator>
    // void GetBSSRDF(TextureEvaluator texEval, MaterialEvalContext ctx,
    //                             SampledWavelengths &lambda, void *) const {}

    static constexpr bool HasSubsurfaceScattering() { return false; }

    std::string ToString() const;

    // DiffuseMaterial(SpectrumTexture reflectance, FloatTexture displacement,
    //                 Image *normalMap)
    //     : normalMap(normalMap), displacement(displacement), reflectance(reflectance) {}

    // template <typename TextureEvaluator>
    // bool CanEvaluateTextures(TextureEvaluator texEval) const {
    //     return texEval.CanEvaluate({}, {reflectance});
    // }

    // template <typename TextureEvaluator>
    // DiffuseBxDF GetBxDF(TextureEvaluator texEval, MaterialEvalContext ctx,
    //                                  SampledWavelengths &lambda) const {
    //     SampledSpectrum r = Clamp(texEval(reflectance, ctx, lambda), 0, 1);
    //     return DiffuseBxDF(r);
    // }
    
    // TODO: need to update GetBSDF() interface
    BSDF GetBSDF(MaterialEvalContext ctx) const {
        Spectrum r; // = reflectance->Evaluate();
        auto bxdf = std::make_shared<BxDF>(r);
        return BSDF(ctx.ns, ctx.dpdus, bxdf);
    }

private:
    // Image *normalMap;
    // FloatTexture displacement;
    std::shared_ptr<Texture> reflectance;
};



} // namespace PBRender