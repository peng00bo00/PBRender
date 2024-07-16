#pragma once

#include <PBRender/core/texture.h>

namespace PBRender
{

// SpectrumConstantTexture Definition
class ConstantTexture : public SpectrumTexture {
public:
    // SpectrumConstantTexture Public Methods
    ConstantTexture(Spectrum value) : value(value) {}

    Spectrum Evaluate(TextureEvalContext ctx) const {
        return value;
    }

    // static SpectrumConstantTexture *Create(const Transform &renderFromTexture,
    //                                        const TextureParameterDictionary &parameters,
    //                                        SpectrumType spectrumType, const FileLoc *loc,
    //                                        Allocator alloc);

    std::string ToString() const;

private:
    Spectrum value;
};


} // namespace PBRender