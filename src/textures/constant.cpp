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

std::shared_ptr<ConstantTexture<float>> CreateConstantFloatTexture(const Transform &tex2world,
                                                   const float tp) {
    return std::make_shared<ConstantTexture<float>>(tp);
}

std::shared_ptr<ConstantTexture<Spectrum>> CreateConstantSpectrumTexture(
    const Transform &tex2world, const float &tp) {
    return std::make_shared<ConstantTexture<Spectrum>>(Spectrum(tp));
}

}