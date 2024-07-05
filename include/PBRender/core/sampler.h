#pragma once

#include <PBRender/core/common.h>
#include <PBRender/core/camera.h>
#include <PBRender/core/vecmath.h>
#include <PBRender/core/rng.h>

namespace PBRender
{

class Sampler {
public:
	virtual ~Sampler() {}
	virtual void StartPixelSample(Point2i p, int sampleIndex, int dimension = 0) = 0;

	virtual float Get1D() = 0;
	virtual Point2f Get2D() = 0;
	virtual Point2f GetPixel2D() = 0;

	virtual std::unique_ptr<Sampler> Clone() = 0;

	virtual std::string ToString() const = 0;
};

template <typename S>
inline CameraSample GetCameraSample(S sampler, Point2i pPixel) {
	CameraSample cs;

	Point2f dxy = sampler->GetPixel2D();
	cs.pFilm = Point2f(pPixel.x + dxy.x, pPixel.y + dxy.y);

	return cs;
}

} // namespace PBRender