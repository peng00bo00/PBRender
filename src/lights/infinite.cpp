#include "lights/infinite.h"
#include "sampling.h"

#include "ext/stbim/stb_image.h"

namespace PBRender {

void get_sphere_uv(const Vector3f &p, float &u, float &v) {
    float phi = atan2(p.z, p.x);
    if (phi < 0.f) phi += 2 * Pi;

    float theta = Pi - acos(p.y);

    u = phi * Inv2Pi;
    v = theta * InvPi;
}

Spectrum SkyBoxLight::Le(const Ray &r) const {

    Vector3f oc = r.o - worldCenter;

    float a = Dot(r.d, r.d);
    float b = 2.0 * Dot(oc, r.d);
    float c = Dot(oc, oc) - worldRadius * worldRadius;
    float discriminant = b*b - 4*a*c;
    float t;

    if (discriminant < 0) return 0.f;

    t = (-b + sqrt(discriminant)) / (2.0 * a);
    Point3f hitPos = r.o + t * r.d;
    Vector3f hitPos_temp = hitPos - worldCenter;

    if (data) {
        float u, v;
        get_sphere_uv(hitPos_temp, u, v);
        return getLightValue(u, v);
    }

    Spectrum Col;

    Col[0] = (hitPos_temp.x + worldRadius) / (2.f * worldRadius);
    Col[1] = (hitPos_temp.y + worldRadius) / (2.f * worldRadius);
    Col[2] = (hitPos_temp.z + worldRadius) / (2.f * worldRadius);

    return Col;
}

bool SkyBoxLight::loadImage(char *imageFile) {
    stbi_set_flip_vertically_on_load(true);

    data = stbi_loadf(imageFile, &imageWidth, &imageHeight, &nrComponents, 0);

    if (data) {
        std::cout << "Successfully load file: " << imageFile << "!" << std::endl;
        return true;
    }
    return false;
}

Spectrum SkyBoxLight::Sample_Li(const Interaction &ref, const Point2f &u,
                                Vector3f *wi, float *pdf,
                                VisibilityTester *vis) const {
    float theta = u.y * Pi;
    float phi = u.x * 2 * Pi;

    float cosTheta = std::cos(theta);
    float sinTheta = std::sin(theta);

    float sinPhi = std::sin(phi);
    float cosPhi = std::cos(phi);

    *wi = LightToWorld(Vector3f(sinTheta * cosPhi, sinTheta * sinPhi, cosTheta));
    *pdf = 1.f / (4 * Pi);
    *vis = VisibilityTester(ref, Interaction(ref.p + *wi * (2 * worldRadius), ref.time));

    return getLightValue(u.x, u.y);
}

// InfiniteAreaLight Method Definitions
InfiniteAreaLight::InfiniteAreaLight(const Transform &LightToWorld,
                                     const Spectrum &L, int nSamples,
                                     const std::string &texmap)
    : Light((int)LightFlags::Infinite, LightToWorld, 
            // MediumInterface(),
            nSamples) {
    // Read texel data from _texmap_ and initialize _Lmap_
    Point2i resolution;
    std::unique_ptr<RGBSpectrum[]> texels(nullptr);

    float *data;
    RGBSpectrum *data_s;

    if (texmap != "") {
        // texels = ReadImage(texmap, &resolution);

        // read with stb
        int imageWidth, imageHeight, nrComponents;
        data = stbi_loadf(texmap.c_str(), &imageWidth, &imageHeight, &nrComponents, 0);
        data_s = new RGBSpectrum[imageWidth * imageHeight];

        for (int i = 0; i < imageWidth; ++i)
        {
            for (int j = 0; j < imageHeight; ++j)
            {
                RGBSpectrum r;

                r[0] = L[0] * data[(i + j * imageWidth) * nrComponents + 0];
                r[1] = L[1] * data[(i + j * imageWidth) * nrComponents + 1];
                r[2] = L[2] * data[(i + j * imageWidth) * nrComponents + 2];

                data_s[i + j * imageWidth] = r;
            }
        }

        resolution.x = imageWidth;
        resolution.y = imageHeight;
        
        std::unique_ptr<RGBSpectrum[]> texels(data_s);
        free(data_s);

        if (texels)
            for (int i = 0; i < resolution.x * resolution.y; ++i)
                texels[i] *= L.ToRGBSpectrum();
    }
    if (!texels) {
        resolution.x = resolution.y = 1;
        texels = std::unique_ptr<RGBSpectrum[]>(new RGBSpectrum[1]);
        texels[0] = L.ToRGBSpectrum();
    }
    Lmap.reset(new MIPMap<RGBSpectrum>(resolution, texels.get()));

    // Initialize sampling PDFs for infinite area light

    // Compute scalar-valued image _img_ from environment map
    int width = 2 * Lmap->Width(), height = 2 * Lmap->Height();
    std::unique_ptr<float[]> img(new float[width * height]);
    float fwidth = 0.5f / std::min(width, height);
    // ParallelFor(
    //     [&](int64_t v) {
    //         float vp = (v + .5f) / (float)height;
    //         float sinTheta = std::sin(Pi * (v + .5f) / height);
    //         for (int u = 0; u < width; ++u) {
    //             float up = (u + .5f) / (float)width;
    //             img[u + v * width] = Lmap->Lookup(Point2f(up, vp), fwidth).y();
    //             img[u + v * width] *= sinTheta;
    //         }
    //     },
    //     height, 32);

    for (int v = 0; v < height; ++v)
    {
        float vp = (v + .5f) / (float)height;
        float sinTheta = std::sin(Pi * (v + .5f) / height);

        for (int u = 0; u < width; ++u) {
            float up = (u + .5f) / (float)width;
            img[u + v * width] = Lmap->Lookup(Point2f(up, vp), fwidth).y();
            img[u + v * width] *= sinTheta;
        }
    }
    

    // Compute sampling distributions for rows and columns of image
    distribution.reset(new Distribution2D(img.get(), width, height));
}

Spectrum InfiniteAreaLight::Power() const {
    return Pi * worldRadius * worldRadius *
           Spectrum(Lmap->Lookup(Point2f(.5f, .5f), .5f),
                    SpectrumType::Illuminant);
}

Spectrum InfiniteAreaLight::Le(const Ray &ray) const {
    Vector3f w = Normalize(WorldToLight(ray.d));
    Point2f st(SphericalPhi(w) * Inv2Pi, SphericalTheta(w) * InvPi);
    return Spectrum(Lmap->Lookup(st), SpectrumType::Illuminant);
}

Spectrum InfiniteAreaLight::Sample_Li(const Interaction &ref, const Point2f &u,
                                      Vector3f *wi, float *pdf,
                                      VisibilityTester *vis) const {
    // ProfilePhase _(Prof::LightSample);
    // Find $(u,v)$ sample coordinates in infinite light texture
    float mapPdf;
    Point2f uv = distribution->SampleContinuous(u, &mapPdf);
    if (mapPdf == 0) return Spectrum(0.f);

    // Convert infinite light sample point to direction
    float theta = uv[1] * Pi, phi = uv[0] * 2 * Pi;
    float cosTheta = std::cos(theta), sinTheta = std::sin(theta);
    float sinPhi = std::sin(phi), cosPhi = std::cos(phi);
    *wi =
        LightToWorld(Vector3f(sinTheta * cosPhi, sinTheta * sinPhi, cosTheta));

    // Compute PDF for sampled infinite light direction
    *pdf = mapPdf / (2 * Pi * Pi * sinTheta);
    if (sinTheta == 0) *pdf = 0;

    // Return radiance value for infinite light direction
    *vis = VisibilityTester(ref, Interaction(ref.p + *wi * (2 * worldRadius),
                                             ref.time));
                                            //  mediumInterface));
    return Spectrum(Lmap->Lookup(uv), SpectrumType::Illuminant);
}

float InfiniteAreaLight::Pdf_Li(const Interaction &, const Vector3f &w) const {
    // ProfilePhase _(Prof::LightPdf);
    Vector3f wi = WorldToLight(w);
    float theta = SphericalTheta(wi), phi = SphericalPhi(wi);
    float sinTheta = std::sin(theta);
    if (sinTheta == 0) return 0;
    return distribution->Pdf(Point2f(phi * Inv2Pi, theta * InvPi)) /
           (2 * Pi * Pi * sinTheta);
}

Spectrum InfiniteAreaLight::Sample_Le(const Point2f &u1, const Point2f &u2,
                                      float time, Ray *ray, Normal3f *nLight,
                                      float *pdfPos, float *pdfDir) const {
    // ProfilePhase _(Prof::LightSample);
    // Compute direction for infinite light sample ray
    Point2f u = u1;

    // Find $(u,v)$ sample coordinates in infinite light texture
    float mapPdf;
    Point2f uv = distribution->SampleContinuous(u, &mapPdf);
    if (mapPdf == 0) return Spectrum(0.f);
    float theta = uv[1] * Pi, phi = uv[0] * 2.f * Pi;
    float cosTheta = std::cos(theta), sinTheta = std::sin(theta);
    float sinPhi = std::sin(phi), cosPhi = std::cos(phi);
    Vector3f d =
        -LightToWorld(Vector3f(sinTheta * cosPhi, sinTheta * sinPhi, cosTheta));
    *nLight = (Normal3f)d;

    // Compute origin for infinite light sample ray
    Vector3f v1, v2;
    CoordinateSystem(-d, &v1, &v2);
    Point2f cd = ConcentricSampleDisk(u2);
    Point3f pDisk = worldCenter + worldRadius * (cd.x * v1 + cd.y * v2);
    *ray = Ray(pDisk + worldRadius * -d, d, Infinity, time);

    // Compute _InfiniteAreaLight_ ray PDFs
    *pdfDir = sinTheta == 0 ? 0 : mapPdf / (2 * Pi * Pi * sinTheta);
    *pdfPos = 1 / (Pi * worldRadius * worldRadius);
    return Spectrum(Lmap->Lookup(uv), SpectrumType::Illuminant);
}

void InfiniteAreaLight::Pdf_Le(const Ray &ray, const Normal3f &, float *pdfPos,
                               float *pdfDir) const {
    // ProfilePhase _(Prof::LightPdf);
    Vector3f d = -WorldToLight(ray.d);
    float theta = SphericalTheta(d), phi = SphericalPhi(d);
    Point2f uv(phi * Inv2Pi, theta * InvPi);
    float mapPdf = distribution->Pdf(uv);
    *pdfDir = mapPdf / (2 * Pi * Pi * std::sin(theta));
    *pdfPos = 1 / (Pi * worldRadius * worldRadius);
}

// std::shared_ptr<InfiniteAreaLight> CreateInfiniteLight(
//     const Transform &light2world, const ParamSet &paramSet) {
//     Spectrum L = paramSet.FindOneSpectrum("L", Spectrum(1.0));
//     Spectrum sc = paramSet.FindOneSpectrum("scale", Spectrum(1.0));
//     std::string texmap = paramSet.FindOneFilename("mapname", "");
//     int nSamples = paramSet.FindOneInt("samples",
//                                        paramSet.FindOneInt("nsamples", 1));
//     if (PbrtOptions.quickRender) nSamples = std::max(1, nSamples / 4);
//     return std::make_shared<InfiniteAreaLight>(light2world, L * sc, nSamples,
//                                                texmap);
// }

}