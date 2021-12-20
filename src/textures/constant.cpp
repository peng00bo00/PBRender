#include "textures/constant.h"

namespace PBRender {

// ConstantTexture Method Definitions
// ConstantTexture<float> *CreateConstantFloatTexture(const Transform &tex2world,
//                                                    const TextureParams &tp) {
//     return new ConstantTexture<float>(tp.FindFloat("value", 1.f));
// }

// ConstantTexture<Spectrum> *CreateConstantSpectrumTexture(
//     const Transform &tex2world, const TextureParams &tp) {
//     return new ConstantTexture<Spectrum>(
//         tp.FindSpectrum("value", Spectrum(1.f)));
// }

ConstantTexture<float> *CreateConstantFloatTexture(const Transform &tex2world,
                                                   const float tp) {
    return new ConstantTexture<float>(tp);
}

ConstantTexture<Spectrum> *CreateConstantSpectrumTexture(
    const Transform &tex2world, const float &tp) {
    return new ConstantTexture<Spectrum>(Spectrum(tp));
}

}