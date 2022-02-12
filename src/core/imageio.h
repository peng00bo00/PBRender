#pragma once

#include "PBRender.h"
#include "geometry.h"

namespace PBRender {

// ImageIO Declarations
std::unique_ptr<RGBSpectrum[]> ReadImage(const std::string &filename, Point2i *resolution);

void WriteImage(const char* filename, const char *rgb,
                const Point2i &fullResolution);

void WriteImage(const char* filename, const char *rgb,
                const Vector2f &fullResolution);

}