#pragma once

#include "PBRender.h"
#include "interaction.h"

namespace PBRender {

// LightFlags Declarations
enum class LightFlags : int {
    DeltaPosition = 1,
    DeltaDirection = 2,
    Area = 4,
    Infinite = 8
};

inline bool IsDeltaLight(int flags) {
    return flags & (int)LightFlags::DeltaPosition ||
           flags & (int)LightFlags::DeltaDirection;
}

class Light {
    public:
        Light(int flags, const Transform &LightToWorld,
            //   const MediumInterface &mediumInterface, 
              int nSamples = 1);
        virtual ~Light();

        virtual Spectrum Power() const = 0;
        virtual void Preprocess(const Scene &scene) {}
        virtual Spectrum Le(const Ray &r) const;
        virtual float Pdf_Li(const Interaction &ref, const Vector3f &wi) const = 0;
        virtual Spectrum Sample_Le(const Point2f &u1, const Point2f &u2, float time,
                                   Ray *ray, Normal3f *nLight, float *pdfPos,
                                   float *pdfDir) const = 0;
        virtual void Pdf_Le(const Ray &ray, const Normal3f &nLight, float *pdfPos,
                            float *pdfDir) const = 0;

    // Light Public Data
    public:
        const int flags;
        const int nSamples;
        // const MediumInterface mediumInterface;
    
    // Light Protected Data
    protected:
        const Transform LightToWorld, WorldToLight;
};

class VisibilityTester {
    public:
        VisibilityTester() {}
        VisibilityTester(const Interaction &p0, const Interaction &p1)
        : p0(p0), p1(p1) {}

        const Interaction &P0() const { return p0; }
        const Interaction &P1() const { return p1; }
        
        bool Unoccluded(const Scene &scene) const;
        Spectrum Tr(const Scene &scene, Sampler &sampler) const;
    
    private:
        Interaction p0, p1;
};

class AreaLight : public Light {
    public:
        AreaLight(const Transform &LightToWorld, 
                //   const MediumInterface &medium,
                  int nSamples);
        
        virtual Spectrum L(const Interaction &intr, const Vector3f &w) const = 0;
};

}