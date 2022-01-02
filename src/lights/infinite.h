#pragma once

#include "PBRender.h"
#include "light.h"
#include "texture.h"
#include "shape.h"
#include "scene.h"
#include "mipmap.h"

namespace PBRender {

class SkyBoxLight : public Light {
    public:
        SkyBoxLight(const Transform &LightToWorld, const Point3f &worldCenter,
                    float worldRadius, int nSamples)
                    : Light((int)LightFlags::Infinite, LightToWorld, nSamples),
                      worldCenter(worldCenter),
                      worldRadius(worldRadius) {
            imageHeight = 0;
            imageWidth  = 0;
            nrComponents= 0;
            data = nullptr;
        }
        
        ~SkyBoxLight() {
            if (data) free(data);
        }

        void Preprocess(const Scene &scene) {}
        Spectrum Power() const { return Spectrum(0.f); }

        Spectrum Le(const Ray &r) const;
        
        Spectrum Sample_Li(const Interaction &ref, const Point2f &u,
                           Vector3f *wi, float *pdf,
                           VisibilityTester *vis) const;
        float Pdf_Li(const Interaction &ref, const Vector3f &wi) const { return 0.0f; }

        Spectrum Sample_Le(const Point2f &u1, const Point2f &u2, float time,
                           Ray *ray, Normal3f *nLight, float *pdfPos,
                           float *pdfDir) const { return Spectrum(0.f); }
        
        void Pdf_Le(const Ray &ray, const Normal3f &nLight, float *pdfPos,
                    float *pdfDir) const {}
        
        bool loadImage(char* imageFile);

        Spectrum getLightValue(float u, float v) const {
            int w = u * imageWidth;
            int h = v * imageHeight;
            int offset = (w + h*imageWidth) * nrComponents;

            Spectrum Lv;
            Lv[0] = data[offset + 0] / 5.f;
            Lv[1] = data[offset + 1] / 5.f;
            Lv[2] = data[offset + 2] / 5.f;

            return Lv;
        }
    
    private:
        Point3f worldCenter;
        float worldRadius;

        int imageWidth, imageHeight, nrComponents;
        float *data;
};

// InfiniteAreaLight Declarations
class InfiniteAreaLight : public Light {
    public:
        // InfiniteAreaLight Public Methods
        InfiniteAreaLight(const Transform &LightToWorld, const Spectrum &power,
                          int nSamples, const std::string &texmap);
        
        void Preprocess(const Scene &scene) {
            scene.WorldBound().BoundingSphere(&worldCenter, &worldRadius);
        }

        Spectrum Power() const;

        // Spectrum Le(const RayDifferential &ray) const;
        Spectrum Le(const Ray &ray) const;

        Spectrum Sample_Li(const Interaction &ref, const Point2f &u, Vector3f *wi,
                           float *pdf, VisibilityTester *vis) const;
        
        float Pdf_Li(const Interaction &, const Vector3f &) const;

        Spectrum Sample_Le(const Point2f &u1, const Point2f &u2, float time,
                           Ray *ray, Normal3f *nLight, float *pdfPos,
                           float *pdfDir) const;
        
        void Pdf_Le(const Ray &, const Normal3f &, float *pdfPos,
                    float *pdfDir) const;
    
    private:
        // InfiniteAreaLight Private Data
        std::unique_ptr<MIPMap<RGBSpectrum>> Lmap;
        Point3f worldCenter;
        float worldRadius;
        std::unique_ptr<Distribution2D> distribution;
};

}