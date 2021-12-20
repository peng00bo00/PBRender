#pragma once

#include "PBRender.h"
#include "texture.h"

namespace PBRender {

template <typename T>
class ConstantTexture : public Texture<T> {
    public:
        // ConstantTexture Public Methods
        ConstantTexture(const T &value) : value(value) {}
        T Evaluate(const SurfaceInteraction &) const { return value; }

    private:
        T value;
};

// ConstantTexture<float> *CreateConstantFloatTexture(const Transform &tex2world,
//                                                    const TextureParams &tp);

// ConstantTexture<Spectrum> *CreateConstantSpectrumTexture(
//     const Transform &tex2world, const TextureParams &tp);

ConstantTexture<float> *CreateConstantFloatTexture(const Transform &tex2world,
                                                   const float tp);

ConstantTexture<Spectrum> *CreateConstantSpectrumTexture(
    const Transform &tex2world, const Spectrum tp);

}