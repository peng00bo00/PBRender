#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif

#ifndef PBRT_CORE_IMAGEIO_H
#define PBRT_CORE_IMAGEIO_H

// core/imageio.h*
#include "pbrt.h"
#include "geometry.h"
#include <cctype>

namespace pbrt {

// ImageIO Declarations
std::unique_ptr<RGBSpectrum[]> ReadImage(const std::string &name,
                                         Point2i *resolution);
RGBSpectrum *ReadImageEXR(const std::string &name, int *width,
                          int *height, Bounds2i *dataWindow = nullptr,
                          Bounds2i *displayWindow = nullptr);

void WriteImage(const std::string &name, const Float *rgb,
                const Bounds2i &outputBounds, const Point2i &totalResolution);

}  // namespace pbrt

#endif  // PBRT_CORE_IMAGEIO_H
