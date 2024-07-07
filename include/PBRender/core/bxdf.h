#pragma once

#include <PBRender/core/common.h>
#include <PBRender/core/vecmath.h>
#include <PBRender/core/spectrum.h>
#include <PBRender/core/sampling.h>

namespace PBRender
{

// BxDFReflTransFlags Definition
enum class BxDFReflTransFlags {
    Unset = 0,
    Reflection = 1 << 0,
    Transmission = 1 << 1,
    All = Reflection | Transmission
};

inline BxDFReflTransFlags operator|(BxDFReflTransFlags a, BxDFReflTransFlags b) {
    return BxDFReflTransFlags((int)a | (int)b);
}

inline int operator&(BxDFReflTransFlags a, BxDFReflTransFlags b) {
    return ((int)a & (int)b);
}

inline BxDFReflTransFlags &operator|=(BxDFReflTransFlags &a, BxDFReflTransFlags b) {
    (int &)a |= int(b);
    return a;
}

std::string ToString(BxDFReflTransFlags flags);

// BxDFFlags Definition
enum BxDFFlags {
    Unset = 0,
    Reflection = 1 << 0,
    Transmission = 1 << 1,
    Diffuse = 1 << 2,
    Glossy = 1 << 3,
    Specular = 1 << 4,
    // Composite _BxDFFlags_ definitions
    DiffuseReflection = Diffuse | Reflection,
    DiffuseTransmission = Diffuse | Transmission,
    GlossyReflection = Glossy | Reflection,
    GlossyTransmission = Glossy | Transmission,
    SpecularReflection = Specular | Reflection,
    SpecularTransmission = Specular | Transmission,
    All = Diffuse | Glossy | Specular | Reflection | Transmission
};

inline BxDFFlags operator|(BxDFFlags a, BxDFFlags b) {
    return BxDFFlags((int)a | (int)b);
}

inline int operator&(BxDFFlags a, BxDFFlags b) {
    return ((int)a & (int)b);
}

inline int operator&(BxDFFlags a, BxDFReflTransFlags b) {
    return ((int)a & (int)b);
}

inline BxDFFlags &operator|=(BxDFFlags &a, BxDFFlags b) {
    (int &)a |= int(b);
    return a;
}

// BxDFFlags Inline Functions
inline bool IsReflective(BxDFFlags f) {
    return f & BxDFFlags::Reflection;
}
inline bool IsTransmissive(BxDFFlags f) {
    return f & BxDFFlags::Transmission;
}
inline bool IsDiffuse(BxDFFlags f) {
    return f & BxDFFlags::Diffuse;
}
inline bool IsGlossy(BxDFFlags f) {
    return f & BxDFFlags::Glossy;
}
inline bool IsSpecular(BxDFFlags f) {
    return f & BxDFFlags::Specular;
}
inline bool IsNonSpecular(BxDFFlags f) {
    return f & (BxDFFlags::Diffuse | BxDFFlags::Glossy);
}

std::string ToString(BxDFFlags flags);

// TransportMode Definition
enum class TransportMode { Radiance, Importance };

inline TransportMode operator!(TransportMode mode) {
    return (mode == TransportMode::Radiance) ? TransportMode::Importance
                                             : TransportMode::Radiance;
}

std::string ToString(TransportMode mode);

struct BSDFSample {
    // BSDFSample Public Methods
    BSDFSample() = default;
    BSDFSample(Spectrum f, Vector3f wi, float pdf, BxDFFlags flags, float eta = 1,
               bool pdfIsProportional = false)
        : f(f),
          wi(wi),
          pdf(pdf),
          flags(flags),
          eta(eta),
          pdfIsProportional(pdfIsProportional) {}

    
    bool IsReflection() const { return PBRender::IsReflective(flags); }
    
    bool IsTransmission() const { return PBRender::IsTransmissive(flags); }
    
    bool IsDiffuse() const { return PBRender::IsDiffuse(flags); }
    
    bool IsGlossy() const { return PBRender::IsGlossy(flags); }
    
    bool IsSpecular() const { return PBRender::IsSpecular(flags); }

    std::string ToString() const;
    Spectrum f;
    Vector3f wi;
    float pdf = 0;
    BxDFFlags flags;
    float eta = 1;
    bool pdfIsProportional = false;
};

class DiffuseBxDF;
class DiffuseTransmissionBxDF;
class DielectricBxDF;
class ThinDielectricBxDF;
class HairBxDF;
class MeasuredBxDF;
class ConductorBxDF;
class NormalizedFresnelBxDF;
class CoatedDiffuseBxDF;
class CoatedConductorBxDF;

// BxDF Definition
class BxDF {
public:
    // BxDF Interface
    virtual BxDFFlags Flags() const = 0;

    virtual std::string ToString() const = 0;

    virtual Spectrum f(Vector3f wo, Vector3f wi, TransportMode mode) const = 0;

    virtual BSDFSample Sample_f(
        Vector3f wo, float uc, Point2f u, TransportMode mode = TransportMode::Radiance,
        BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const = 0;

    virtual float PDF(
        Vector3f wo, Vector3f wi, TransportMode mode,
        BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const = 0;

    // rho() interface
    Spectrum rho(Vector3f wo, std::vector<float> &uc, std::vector<Point2f> &u2) const;
    Spectrum rho(std::vector<Point2f> &u1, std::vector<float> &uc2, std::vector<Point2f> &u2) const;

    virtual void Regularize() = 0;
};


} // namespace PBRender