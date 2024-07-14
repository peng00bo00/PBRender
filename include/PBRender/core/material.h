#pragma once

#include <PBRender/core/common.h>
#include <PBRender/core/bsdf.h>
#include <PBRender/core/texture.h>

namespace PBRender
{

struct MaterialEvalContext : public TextureEvalContext {
    // MaterialEvalContext Public Methods
    MaterialEvalContext() = default;
    // MaterialEvalContext(const SurfaceInteraction &si)
    //     : TextureEvalContext(si), wo(si.wo), ns(si.shading.n), dpdus(si.shading.dpdu) {}
    std::string ToString() const;

    Vector3f wo;
    Normal3f ns;
    Vector3f dpdus;
};

// Material Declarations
class CoatedDiffuseMaterial;
class CoatedConductorMaterial;
class ConductorMaterial;
class DielectricMaterial;
class DiffuseMaterial;
class DiffuseTransmissionMaterial;
class HairMaterial;
class MeasuredMaterial;
class SubsurfaceMaterial;
class ThinDielectricMaterial;
class MixMaterial;


class Material {
public:
    virtual std::string ToString() const = 0;

    virtual BSDF GetBSDF(MaterialEvalContext ctx) const = 0;

    // TODO: update other member functions
    // inline const Image *GetNormalMap() const;

    // inline FloatTexture GetDisplacement() const;

    // inline bool HasSubsurfaceScattering() const;
};

} // namespace PBRender