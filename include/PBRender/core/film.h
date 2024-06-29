#pragma once

#include <PBRender/core/common.h>
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
    Bounds2i FullFrame() const { return {Point2i(0, 0), fullResolution}; };
    // Bounds2i PixelBounds() const { return pixelBounds; }
    // float Diagonal() const { return diagonal; }
    // Filter GetFilter() const { return filter; }
    // const PixelSensor *GetPixelSensor() const { return sensor; }
    std::string GetFilename() const { return filename; }

protected:
    Point2i fullResolution;             // resolution of the film
    // Bounds2i pixelBounds;            // pixel bounds
    // Filter filter;                   // pixel filter, skipped for now
    // float diagonal;                  // diagnoal size of the film (in meter)
    // const PixelSensor *sensor;       // pixel sensor, skipped for now
    std::string filename;               // file name
};

class RGBFilm : public Film {

};

class GeometryFilm : public Film {
public:
    GeometryFilm(Point2i fullResolution, std::string filename);
    ~GeometryFilm();

    void AddSample();
    void AddSplat();

    static void *Create();

    void WriteImage();

private:
    struct Pixel {
        Pixel() = default;

        float rgbSum[3] = {0., 0., 0.};
        float weightSum = 0., gBufferWeightSum = 0.;

        float dzdxSum = 0, dzdySum = 0;
        Normal3f nSum, nsSum;
        Point2f uvSum;
        float rgbAlbedoSum[3] = {0., 0., 0.};
    };

    std::unique_ptr<Array2D<Pixel>> data;
};

} // namespace PBRender