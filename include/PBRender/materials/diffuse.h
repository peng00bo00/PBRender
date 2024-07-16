#pragma once

#include <PBRender/core/material.h>
#include <PBRender/bxdfs/diffuse.h>

namespace PBRender
{

class DiffuseMaterial : public Material {
  public:
    // DiffuseMaterial Type Definitions
    using BxDF = DiffuseBxDF;
    using BSSRDF = void;

    DiffuseMaterial(std::shared_ptr<SpectrumTexture> reflectance): reflectance(reflectance) {}

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
        Spectrum r = reflectance->Evaluate(ctx);
        auto bxdf = std::make_shared<BxDF>(r);
        return BSDF(ctx.ns, ctx.dpdus, bxdf);
    }

private:
    // Image *normalMap;
    // FloatTexture displacement;
    std::shared_ptr<SpectrumTexture> reflectance;
};

class DiffuseTransmissionMaterial : public Material {
  public:
    using BxDF = DiffuseTransmissionBxDF;
    using BSSRDF = void;
    // DiffuseTransmissionMaterial Public Methods
    // DiffuseTransmissionMaterial(SpectrumTexture reflectance,
    //                             SpectrumTexture transmittance, FloatTexture displacement,
    //                             Image *normalMap, Float scale)
    //     : displacement(displacement),
    //       normalMap(normalMap),
    //       reflectance(reflectance),
    //       transmittance(transmittance),
    //       scale(scale) {}

    static const char *Name() { return "DiffuseTransmissionMaterial"; }

    // template <typename TextureEvaluator>
    // bool CanEvaluateTextures(TextureEvaluator texEval) const {
    //     return texEval.CanEvaluate({}, {reflectance, transmittance});
    // }

    BSDF GetBSDF(MaterialEvalContext ctx) const {
        Spectrum r = reflectance->Evaluate(ctx);
        Spectrum t = transmittance->Evaluate(ctx);
        auto bxdf = std::make_shared<BxDF>(r, t);
        return BSDF(ctx.ns, ctx.dpdus, bxdf);
    }

    // FloatTexture GetDisplacement() const { return displacement; }
    // const Image *GetNormalMap() const { return normalMap; }

    // static DiffuseTransmissionMaterial *Create(
    //     const TextureParameterDictionary &parameters, Image *normalMap,
    //     const FileLoc *loc, Allocator alloc);

    // template <typename TextureEvaluator>
    // void GetBSSRDF(TextureEvaluator texEval, MaterialEvalContext ctx,
    //                             SampledWavelengths &lambda) const {}

    static constexpr bool HasSubsurfaceScattering() { return false; }

    std::string ToString() const;

  private:
    // DiffuseTransmissionMaterial Private Data
    // FloatTexture displacement;
    // Image *normalMap;
    std::shared_ptr<SpectrumTexture> reflectance, transmittance;
    float scale;
};


} // namespace PBRender