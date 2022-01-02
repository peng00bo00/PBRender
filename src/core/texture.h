#pragma once

#include "PBRender.h"
#include "spectrum.h"
#include "geometry.h"
#include "transform.h"

namespace PBRender {

template <typename T>
class Texture {
    public:
		// Texture Interface
		virtual T Evaluate(const SurfaceInteraction &) const = 0;
		virtual ~Texture() {}
};

float Lanczos(float, float tau = 2);
float Noise(float x, float y = .5f, float z = .5f);
float Noise(const Point3f &p);
float FBm(const Point3f &p, const Vector3f &dpdx, const Vector3f &dpdy,
          float omega, int octaves);
float Turbulence(const Point3f &p, const Vector3f &dpdx, const Vector3f &dpdy,
                 float omega, int octaves);

}