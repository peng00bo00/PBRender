#pragma once

#include <PBRender/core/common.h>
#include <PBRender/core/spectrum.h>
#include <PBRender/core/container.h>

namespace PBRender
{

class Film {
public:
    Film(Point2i fullResolution, 
         //  float diagonal, 
         std::string filename) :
         fullResolution(fullResolution), 
         // diagonal(diagonal * 0.001f), 
         filename(filename) { }

    Point2i FullResolution() const { return fullResolution; }
    Bounds2i FullFrameBound() const { return {Point2i(0, 0), fullResolution}; };
    // Bounds2i PixelBounds() const { return pixelBounds; }
    // float Diagonal() const { return diagonal; }
    // Filter GetFilter() const { return filter; }
    // const PixelSensor *GetPixelSensor() const { return sensor; }
    std::string GetFilename() const { return filename; }

    // TODO: update L to color spectrum
    virtual void AddSample(Point2i pFilm, const Spectrum &L) = 0;
    virtual Spectrum GetPixel(const Point2i pFilm) const = 0;
    virtual void WriteImage() const = 0;

protected:
    Point2i fullResolution;             // resolution of the film
    // Bounds2i pixelBounds;            // pixel bounds
    // Filter filter;                   // pixel filter, skipped for now
    // float diagonal;                  // diagnoal size of the film (in meter)
    // const PixelSensor *sensor;       // pixel sensor, skipped for now
    std::string filename;               // file name
};

// TODO: update RGBFilm as needed
class RGBFilm : public Film {
public:
    RGBFilm(Point2i fullResolution, std::string filename);
    ~RGBFilm();

    void AddSample(const Point2i pFilm, const Spectrum &L);
    Spectrum GetPixel(const Point2i pFilm) const;
    // void AddSplat();

    static void *Create();

    void WriteImage() const;

private:
    struct Pixel {
        Pixel() = default;

        float rgbSum[3] = {0., 0., 0.};
        float weightSum = 0.f;
        // float gBufferWeightSum = 0.;

        // float dzdxSum = 0, dzdySum = 0;
        // Normal3f nSum, nsSum;
        // Point2f uvSum;
    };

    std::unique_ptr<Array2D<Pixel>> data;
};

class GeometryFilm : public Film {
public:
    GeometryFilm(Point2i fullResolution, std::string filename);
    ~GeometryFilm();

    void AddSample(const Point2i pFilm, const Spectrum &L);
    Spectrum GetPixel(const Point2i pFilm) const;
    // void AddSplat();

    static void *Create();

    void WriteImage() const;

private:
    struct Pixel {
        Pixel() = default;

        float rgbSum[3] = {0., 0., 0.};
        float weightSum = 0.f;
        // float gBufferWeightSum = 0.;

        // float dzdxSum = 0, dzdySum = 0;
        // Normal3f nSum, nsSum;
        // Point2f uvSum;
    };

    std::unique_ptr<Array2D<Pixel>> data;
};

} // namespace PBRender